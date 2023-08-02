#include <avr/interrupt.h>
#include <stddef.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "explore_impl.hh"
#include "firmware/config.hh"
#include "firmware/lib/control/linear.hh"
#include "firmware/lib/control/plan.hh"
#include "firmware/lib/control/position.hh"
#include "firmware/lib/control/speed.hh"
#include "firmware/lib/control/walls.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/dequeue.hh"
#include "firmware/platform/platform.hh"

namespace mouse::mode::explore {

namespace {
Orientation                                 orientation_;  // The current orientation of the mouse.
float                                       cell_offset_;  // The offset into the current cell.
bool                                        stopped_;      // Whether or not the mouse has stopped.
dequeue::Dequeue<maze::PackedLocation, 256> stacks_;       // Front is the breadcrumb trail, Back are unvisited cells.
dequeue::Dequeue<DequeueUpdate, 8>          updates_;      // The queue of updates to send to the host.
}  // namespace

void Explore() {
  stacks_.RegisterCallback([](dequeue::Event event, maze::PackedLocation value) {
    assert(assert::Module::Explore, 8, !updates_.Full());
    switch (event) {
      case dequeue::Event::PushFront:
      case dequeue::Event::PopFront:
        updates_.PushBack((DequeueUpdate){.dequeue_id = DequeueID::Path, .event = event, .value = value});
        break;
      case dequeue::Event::PushBack:
      case dequeue::Event::PopBack:
        updates_.PushBack((DequeueUpdate){.dequeue_id = DequeueID::Next, .event = event, .value = value});
        break;
    }
  });

  // Idle the mouse and turn on the IR LEDs.
  control::plan::SubmitAndWait((control::plan::Plan){
      .type = control::plan::Type::Idle, .state = control::plan::State::Scheduled, .data = {.idle = {}}});
  control::plan::SubmitAndWait((control::plan::Plan){
      .type = control::plan::Type::IR, .state = control::plan::State::Scheduled, .data = {.ir = {true}}});

  // Assumption:
  // We start centered along the back wall of the starting square, with our back touching the wall.
  // Therefore our "position", measured by the center of the axle is kAxleOffset from the wall.
  orientation_ = Orientation::North;
  cell_offset_ = config::kAxleOffset;
  stopped_     = true;

  // Our path so far is just the starting square, and we want to visit the square to our north.
  stacks_.PushFront(maze::Location(0, 0));
  stacks_.PushBack(maze::Location(0, 1));

  // While we have squares to visit...
  while (stacks_.PeekBack() != maze::Location(0, 0)) {
    auto curr_loc = stacks_.PeekFront();
    auto next_loc = stacks_.PeekBack();
    auto prev_loc = maze::Location(0, 0);

    // Determine which direction to drive to reach the cell (if it is adjancent).
    Orientation next_orientation = Adjacent(curr_loc, next_loc);

    // If we are adjacent to the next cell
    if (next_orientation != Orientation::Invalid) {
      stacks_.PopBack();  // Remove the cell from the "next" stack.
      // If we've visited the cell since it was added to the queue, then skip it.
      if (maze::Visited(next_loc)) {
        continue;
      }
      Face(next_orientation);   // Turn to face the cell.
      Advance(next_loc, true);  // Advance into it, updating the breadcrumb trail.
      Classify(next_loc);       // Update our maze representation, and note any new cells to visit.
    } else {
      // Otherwise, backtrack a square.
      stacks_.PopFront();                  // Remove the current cell from the breadcrumb trail.
      prev_loc = stacks_.PeekFront();      // Check which cell we came from.
      Face(Adjacent(curr_loc, prev_loc));  // Turn to face it.
      Advance(prev_loc, false);            // Advance into it, but do NOT update the breadcrumb trail.
    }
  }

  // Now that exploration is complete, we return to the starting cell.
  if (!stacks_.Empty()) {
    auto curr = stacks_.PopFront();
    while (!stacks_.Empty()) {
      auto prev = stacks_.PopFront();
      Face(Adjacent(curr, prev));
      Advance(prev, false);
      curr = prev;
    }
  }

  // Stop in the middle of the last square, facing north.
  Stop();
  Face(Orientation::North);

  // Ensure the control system is idling (no motor activity).
  control::plan::SubmitAndWait((control::plan::Plan){
      .type = control::plan::Type::Idle, .state = control::plan::State::Scheduled, .data = {.idle = {}}});

  // Deregister dequeue callbacks.
  stacks_.RegisterCallback(NULL);

  // Solve the maze, and tramit the new maze data to the host.
  Floodfill();
  maze::Send();
}

// GetReport() is the report handler for the explore mode.
uint8_t GetReport(uint8_t *buffer, uint8_t len) {
  assert(assert::Module::Explore, 0, buffer != NULL);
  assert(assert::Module::Explore, 1, len >= (sizeof(DequeueUpdate) * 16));

  uint8_t i          = 0;
  uint8_t report_len = 0;

  auto *updates_buffer = (DequeueUpdate *)buffer;
  while (!updates_.Empty()) {
    updates_buffer[i++]  = updates_.PopFront();
    report_len          += sizeof(DequeueUpdate);
  }

  return report_len;
}

// adjacent determines the orientation needed to drive between two adjacent cells.
Orientation Adjacent(maze::Location a, maze::Location b) {
  auto ax = a.x;
  auto ay = a.y;
  auto bx = b.x;
  auto by = b.y;

  if (ax == bx) {
    if (ay + 1 == by) {
      if (maze::WallNorth(a)) {
        return Orientation::Invalid;
      } else {
        return Orientation::North;
      }
    } else if (ay - 1 == by) {
      if (maze::WallSouth(a)) {
        return Orientation::Invalid;
      } else {
        return Orientation::South;
      }
    } else {
      return Orientation::Invalid;
    }
  } else if (ay == by) {
    if (ax + 1 == bx) {
      if (maze::WallEast(a)) {
        return Orientation::Invalid;
      } else {
        return Orientation::East;
      }
    } else if (ax - 1 == bx) {
      if (maze::WallWest(a)) {
        return Orientation::Invalid;
      } else {
        return Orientation::West;
      }
    } else {
      return Orientation::Invalid;
    }
  } else {
    return Orientation::Invalid;
  }
}

// face positions the mouse in the middle of the current cell, facing the given orientation.
void Face(Orientation new_orientation) {
  assert(assert::Module::Explore, 2, new_orientation != Orientation::Invalid);

  if (new_orientation == orientation_) {
    return;
  }

  // stop in the middle of the cell
  Stop();

  // determine the delta between the current orientation and the desired orientation
  // Note: The (+4) and (&3) is to ensure the result is in [0,3] without using an expensive % operator.
  uint8_t delta  = (static_cast<uint8_t>(new_orientation) + 4) - static_cast<uint8_t>(orientation_);
  delta         &= 3;

  switch (delta) {
    case 0:
      break;
    case 1:
      control::plan::SubmitAndWait(  //
          (control::plan::Plan){.type  = control::plan::Type::RotationalMotion,
                                .state = control::plan::State::Scheduled,
                                .data  = {.rotational = {
                                              .d_theta = -M_PI_2,
                                         }}});
      break;
    case 2:
      control::plan::SubmitAndWait(  //
          (control::plan::Plan){.type  = control::plan::Type::RotationalMotion,
                                .state = control::plan::State::Scheduled,
                                .data  = {.rotational = {
                                              .d_theta = M_PI,
                                         }}});
      break;
    case 3:
      control::plan::SubmitAndWait(  //
          (control::plan::Plan){.type  = control::plan::Type::RotationalMotion,
                                .state = control::plan::State::Scheduled,
                                .data  = {.rotational = {
                                              .d_theta = M_PI_2,
                                         }}});
      break;
  }

  float position, theta;
  control::position::Tare(position, theta);

  // assuming we were centered horizontally in the previous direction of travel
  // then we are now in the middle of the cell along the new direction of travel.
  cell_offset_ = config::kCellSize2;
  orientation_ = new_orientation;
}

// advance moves 1 square in the current direction, leaving the mouse
// in a position where the sensors are pointed at the centers of the side walls.
void Advance(maze::Location loc, bool update_path) {
  UpdateLocation();

  control::plan::SubmitAndWait(  //
      (control::plan::Plan){.type  = control::plan::Type::LinearMotion,
                            .state = control::plan::State::Scheduled,
                            .data  = {.linear = {
                                          .position = config::kCellSize - (cell_offset_ - kEntryOffset),
                                          .stop     = false,
                                     }}});

  if (update_path) {
    stacks_.PushFront(loc);
  }
  stopped_ = false;
}

// stop stops the mouse in the middle of the current cell.
void Stop() {
  if (stopped_) {
    return;
  }

  UpdateLocation();

  // we should never decide to stop once we have already passed the center of a cell.
  assert(assert::Module::Explore, 3, cell_offset_ <= config::kCellSize2);

  // stop at the center of the cell
  control::plan::SubmitAndWait(                                          //
      (control::plan::Plan){.type  = control::plan::Type::LinearMotion,  //
                            .state = control::plan::State::Scheduled,    //
                            .data  = {.linear = {
                                          .position = config::kCellSize2 - cell_offset_,
                                          .stop     = true  //
                                     }}});

  stopped_ = true;
}

// UpdateLocation updates the cell index and offset based on the traveled distance.
void UpdateLocation() {
  float position_distance, position_theta;
  control::position::Tare(position_distance, position_theta);
  cell_offset_ += position_distance;

  // this is a (% kCellSize) without using the % operator because it is expensive.
  while (cell_offset_ > config::kCellSize) {
    cell_offset_ -= config::kCellSize;
  }
}

// queue_unvisited adds a cell to the "next" stack if it has not already been visited.
void queue_unvisited(maze::Location loc) {
  if (!maze::Visited(loc)) {
    stacks_.PushBack(loc);
  }
}

// classify updates the state of a cell, and adds unvisited neighbors to the "next" stack.
void Classify(maze::Location loc) {
  assert(assert::Module::Explore, 4, orientation_ != Orientation::Invalid);

  bool wall_forward, wall_left, wall_right;
  control::walls::Present(wall_left, wall_right, wall_forward);
  maze::SetVisited(loc);

  switch (orientation_) {
    case Orientation::North:
      if (wall_right) {
        maze::SetWallEast(loc);
      } else {
        queue_unvisited(loc + maze::Location(1, 0));
      }
      if (wall_left) {
        maze::SetWallWest(loc);
      } else {
        queue_unvisited(loc - maze::Location(1, 0));
      }
      if (wall_forward) {
        maze::SetWallNorth(loc);
      } else {
        queue_unvisited(loc + maze::Location(0, 1));
      }
      break;
    case Orientation::East:
      if (wall_right) {
        maze::SetWallSouth(loc);
      } else {
        queue_unvisited(loc - maze::Location(0, 1));
      }
      if (wall_left) {
        maze::SetWallNorth(loc);
      } else {
        queue_unvisited(loc + maze::Location(0, 1));
      }
      if (wall_forward) {
        maze::SetWallEast(loc);
      } else {
        queue_unvisited(loc + maze::Location(1, 0));
      }
      break;
    case Orientation::South:
      if (wall_right) {
        maze::SetWallWest(loc);
      } else {
        queue_unvisited(loc - maze::Location(1, 0));
      }
      if (wall_left) {
        maze::SetWallEast(loc);
      } else {
        queue_unvisited(loc + maze::Location(1, 0));
      }
      if (wall_forward) {
        maze::SetWallSouth(loc);
      } else {
        queue_unvisited(loc - maze::Location(0, 1));
      }
      break;
    case Orientation::West:
      if (wall_right) {
        maze::SetWallNorth(loc);
      } else {
        queue_unvisited(loc + maze::Location(0, 1));
      }
      if (wall_left) {
        maze::SetWallSouth(loc);
      } else {
        queue_unvisited(loc - maze::Location(0, 1));
      }
      if (wall_forward) {
        maze::SetWallWest(loc);
      } else {
        queue_unvisited(loc - maze::Location(1, 0));
      }
      break;
    case Orientation::Invalid:
      break;
  }

  maze::Updated(loc);
}

// Floodfill calculates the shortest path to the goal.
void Floodfill() {
  // Step 1. Find the 2x2 square of cells with no internal walls that is the goal.
  auto goal = maze::Location(15, 15);
  for (uint8_t x = 0; x < config::kMazeWidth - 1; x++) {
    for (uint8_t y = 0; y < config::kMazeHeight - 1; y++) {
      auto a_visited = maze::Visited(maze::Location(x, y));
      auto a_east    = maze::WallEast(maze::Location(x, y));
      auto a_north   = maze::WallNorth(maze::Location(x, y));
      auto b_visited = maze::Visited(maze::Location(x + 1, y + 1));
      auto b_west    = maze::WallWest(maze::Location(x + 1, y + 1));
      auto b_south   = maze::WallSouth(maze::Location(x + 1, y + 1));
      if (a_visited && b_visited && !a_east && !a_north && !b_west && !b_south) {
        goal = maze::Location(x, y);
        break;
      }
    }
  }
  if (goal == 0xFF) {
    // No goal found.
    return;
  }

  // Step 2. Find the cell in the goal square with the gateway, as that's where we want to get.
  if (!maze::WallNorth(goal + maze::Location(0, 1)) || !maze::WallWest(goal + maze::Location(0, 1))) {
    goal += maze::Location(0, 1);
  } else if (!maze::WallSouth(goal + maze::Location(1, 0)) || !maze::WallEast(goal + maze::Location(1, 0))) {
    goal += maze::Location(1, 0);
  } else if (!maze::WallNorth(goal + maze::Location(1, 1)) || !maze::WallEast(goal + maze::Location(1, 1))) {
    goal += maze::Location(1, 1);
  }

  // Step 3. Floodfill outwards from the goal cell.
  stacks_.Clear();
  maze::SetDistance(goal, 0);
  stacks_.PushBack(goal);
  while (!stacks_.Empty()) {
    maze::Location loc = stacks_.PopFront();
    if (!maze::WallNorth(loc)) {
      auto next = loc + maze::Location(0, 1);
      if (maze::Distance(next) == 0xFF) {
        maze::SetDistance(next, maze::Distance(loc) + 1);
        stacks_.PushBack(next);
      }
    }
    if (!maze::WallEast(loc)) {
      auto next = loc + maze::Location(1, 0);
      if (maze::Distance(next) == 0xFF) {
        maze::SetDistance(next, maze::Distance(loc) + 1);
        stacks_.PushBack(next);
      }
    }
    if (!maze::WallSouth(loc)) {
      auto next = loc - maze::Location(0, 1);
      if (maze::Distance(next) == 0xFF) {
        maze::SetDistance(next, maze::Distance(loc) + 1);
        stacks_.PushBack(next);
      }
    }
    if (!maze::WallWest(loc)) {
      auto next = loc - maze::Location(1, 0);
      if (maze::Distance(next) == 0xFF) {
        maze::SetDistance(next, maze::Distance(loc) + 1);
        stacks_.PushBack(next);
      }
    }
  }
}

// solve follows the shortest path to the goal.
void Solve() {
  auto curr     = maze::Location(0, 0);
  auto distance = maze::Distance(curr);

  while (distance != 0) {
    auto north = curr + maze::Location(0, 1);
    auto east  = curr + maze::Location(1, 0);
    auto south = curr - maze::Location(0, 1);
    auto west  = curr - maze::Location(1, 0);

    auto next = curr;

    if (curr.y < (config::kMazeHeight - 1) && !maze::WallNorth(curr) && maze::Distance(north) < distance) {
      next     = north;
      distance = maze::Distance(north);
    }

    if (curr.x < (config::kMazeWidth - 1) && !maze::WallEast(curr) && maze::Distance(east) < distance) {
      next     = east;
      distance = maze::Distance(east);
    }

    if (curr.y > 0 && !maze::WallSouth(curr) && maze::Distance(south) < distance) {
      next     = south;
      distance = maze::Distance(south);
    }

    if (curr.x > 0 && !maze::WallWest(curr) && maze::Distance(west) < distance) {
      next     = west;
      distance = maze::Distance(west);
    }

    if (next == curr) {
      // No path to the goal.
      return;
    }

    Face(Adjacent(curr, next));
    Advance(next, false);
    curr = next;
  }

  // Stop in the middle of the goal cell.
  Stop();

  // Ensure the control system is idling (no motor activity).
  control::plan::SubmitAndWait((control::plan::Plan){
      .type = control::plan::Type::Idle, .state = control::plan::State::Scheduled, .data = {.idle = {}}});
}

}  // namespace mouse::mode::explore
