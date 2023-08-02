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
Orientation                           orientation;  // The current orientation of the mouse.
float                                 cell_offset;  // The offset into the current cell.
bool                                  stopped;      // Whether or not the mouse has stopped.
dequeue::Dequeue<maze::Location, 256> stacks;       // Front is the breadcrumb trail, Back are unvisited cells.
dequeue::Dequeue<DequeueUpdate, 16>   updates;      // The queue of updates to send to the host.
}  // namespace

void Explore() {
  stacks.RegisterCallback([](dequeue::Event event, maze::Location value) {
    assert(assert::Module::Explore, 8, !updates.Full());
    switch (event) {
      case dequeue::Event::PushFront:
      case dequeue::Event::PopFront:
        updates.PushBack((DequeueUpdate){.dequeue_id = DequeueID::Path, .event = event, .value = value});
        break;
      case dequeue::Event::PushBack:
      case dequeue::Event::PopBack:
        updates.PushBack((DequeueUpdate){.dequeue_id = DequeueID::Next, .event = event, .value = value});
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
  orientation = Orientation::North;
  cell_offset = config::kAxleOffset;
  stopped     = true;

  // Our path so far is just the starting square, and we want to visit the square to our north.
  stacks.PushFront(maze::Location(0, 0));
  stacks.PushBack(maze::Location(0, 1));

  // While we have squares to visit...
  while (stacks.PeekBack() != maze::Location(0, 0)) {
    // Skips cells we already visited since they were added to the stack.
    while (stacks.PeekBack() != maze::Location(0, 0) && maze::Read(stacks.PeekBack()).visited) {
      stacks.PopBack();
    }

    // If we have no more cells to visit, then we are done.
    if (stacks.PeekBack() == maze::Location(0, 0)) {
      break;
    }

    auto curr_loc = stacks.PeekFront();
    auto next_loc = stacks.PeekBack();
    auto prev_loc = maze::Location(0, 0);

    // Determine which direction to drive to reach the cell (if it is adjancent).
    Orientation next_orientation = Adjacent(curr_loc, next_loc);

    // If we are adjacent to the next cell
    if (next_orientation != Orientation::Invalid) {
      stacks.PopBack();         // Remove the cell from the "next" stack.
      Face(next_orientation);   // Turn to face the cell.
      Advance(next_loc, true);  // Advance into it, updating the breadcrumb trail.
      Classify(next_loc);       // Update our maze representation, and note any new cells to visit.
    } else {
      // Otherwise, backtrack a square.
      stacks.PopFront();                   // Remove the current cell from the breadcrumb trail.
      prev_loc = stacks.PeekFront();       // Check which cell we came from.
      Face(Adjacent(curr_loc, prev_loc));  // Turn to face it.
      Advance(prev_loc, false);            // Advance into it, but do NOT update the breadcrumb trail.
    }
  }

  // Now that exploration is complete, we return to the starting cell.
  if (!stacks.Empty()) {
    auto curr = stacks.PopFront();
    while (!stacks.Empty()) {
      auto prev = stacks.PopFront();
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
  stacks.RegisterCallback(NULL);

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
  while (!updates.Empty()) {
    updates_buffer[i++]  = updates.PopFront();
    report_len          += sizeof(DequeueUpdate);
  }

  return report_len;
}

// adjacent determines the orientation needed to drive between two adjacent cells.
Orientation Adjacent(maze::Location a, maze::Location b) {
  auto ax = a.X();
  auto ay = a.Y();
  auto bx = b.X();
  auto by = b.Y();

  if (ax == bx) {
    if (ay + 1 == by) {
      if (maze::Read(a).wall_north) {
        return Orientation::Invalid;
      } else {
        return Orientation::North;
      }
    } else if (ay - 1 == by) {
      if (maze::Read(a).wall_south) {
        return Orientation::Invalid;
      } else {
        return Orientation::South;
      }
    } else {
      return Orientation::Invalid;
    }
  } else if (ay == by) {
    if (ax + 1 == bx) {
      if (maze::Read(a).wall_east) {
        return Orientation::Invalid;
      } else {
        return Orientation::East;
      }
    } else if (ax - 1 == bx) {
      if (maze::Read(a).wall_west) {
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

  if (new_orientation == orientation) {
    return;
  }

  // stop in the middle of the cell
  Stop();

  // determine the delta between the current orientation and the desired orientation
  // Note: The (+4) and (&3) is to ensure the result is in [0,3] without using an expensive % operator.
  uint8_t delta  = (static_cast<uint8_t>(new_orientation) + 4) - static_cast<uint8_t>(orientation);
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
  cell_offset = config::kCellSize2;
  orientation = new_orientation;
}

// advance moves 1 square in the current direction, leaving the mouse
// in a position where the sensors are pointed at the centers of the side walls.
void Advance(maze::Location loc, bool update_path) {
  UpdateLocation();

  control::plan::SubmitAndWait(  //
      (control::plan::Plan){.type  = control::plan::Type::LinearMotion,
                            .state = control::plan::State::Scheduled,
                            .data  = {.linear = {
                                          .position = config::kCellSize - (cell_offset - kEntryOffset),
                                          .stop     = false,
                                     }}});

  if (update_path) {
    stacks.PushFront(loc);
  }
  stopped = false;
}

// stop stops the mouse in the middle of the current cell.
void Stop() {
  if (stopped) {
    return;
  }

  UpdateLocation();

  // we should never decide to stop once we have already passed the center of a cell.
  assert(assert::Module::Explore, 3, cell_offset <= config::kCellSize2);

  // stop at the center of the cell
  control::plan::SubmitAndWait(                                          //
      (control::plan::Plan){.type  = control::plan::Type::LinearMotion,  //
                            .state = control::plan::State::Scheduled,    //
                            .data  = {.linear = {
                                          .position = config::kCellSize2 - cell_offset,
                                          .stop     = true  //
                                     }}});

  stopped = true;
}

// UpdateLocation updates the cell index and offset based on the traveled distance.
void UpdateLocation() {
  float position_distance, position_theta;
  control::position::Tare(position_distance, position_theta);
  cell_offset += position_distance;

  // this is a (% kCellSize) without using the % operator because it is expensive.
  while (cell_offset > config::kCellSize) {
    cell_offset -= config::kCellSize;
  }
}

// queue_unvisited adds a cell to the "next" stack if it has not already been visited.
void queue_unvisited(maze::Location loc) {
  if (!maze::Read(loc).visited) {
    stacks.PushBack(loc);
  }
}

// classify updates the state of a cell, and adds unvisited neighbors to the "next" stack.
void Classify(maze::Location loc) {
  assert(assert::Module::Explore, 4, orientation != Orientation::Invalid);

  bool wall_forward, wall_left, wall_right;
  control::walls::Present(wall_left, wall_right, wall_forward);

  // Classify the square based on sensor readings.
  maze::Cell cell = {
      .wall_north = false,
      .wall_east  = false,
      .wall_south = false,
      .wall_west  = false,
      .visited    = true,
      .padding    = 0,
      .distance   = 0xFF,
  };
  switch (orientation) {
    case Orientation::North:
      if (wall_right) {
        cell.wall_east = wall_right;
      } else {
        queue_unvisited(loc + maze::Location(1, 0));
      }
      if (wall_left) {
        cell.wall_west = wall_left;
      } else {
        queue_unvisited(loc - maze::Location(1, 0));
      }
      if (wall_forward) {
        cell.wall_north = wall_forward;
      } else {
        queue_unvisited(loc + maze::Location(0, 1));
      }
      break;
    case Orientation::East:
      if (wall_right) {
        cell.wall_south = wall_right;
      } else {
        queue_unvisited(loc - maze::Location(0, 1));
      }
      if (wall_left) {
        cell.wall_north = wall_left;
      } else {
        queue_unvisited(loc + maze::Location(0, 1));
      }
      if (wall_forward) {
        cell.wall_east = wall_forward;
      } else {
        queue_unvisited(loc + maze::Location(1, 0));
      }
      break;
    case Orientation::South:
      if (wall_right) {
        cell.wall_west = wall_right;
      } else {
        queue_unvisited(loc - maze::Location(1, 0));
      }
      if (wall_left) {
        cell.wall_east = wall_left;
      } else {
        queue_unvisited(loc + maze::Location(1, 0));
      }
      if (wall_forward) {
        cell.wall_south = wall_forward;
      } else {
        queue_unvisited(loc - maze::Location(0, 1));
      }
      break;
    case Orientation::West:
      if (wall_right) {
        cell.wall_north = wall_right;
      } else {
        queue_unvisited(loc + maze::Location(0, 1));
      }
      if (wall_left) {
        cell.wall_south = wall_left;
      } else {
        queue_unvisited(loc - maze::Location(0, 1));
      }
      if (wall_forward) {
        cell.wall_west = wall_forward;
      } else {
        queue_unvisited(loc - maze::Location(1, 0));
      }
      break;
    case Orientation::Invalid:
      break;
  }

  maze::Write(loc, cell);
}

// Floodfill calculates the shortest path to the goal.
void Floodfill() {
  // Step 1. Find the 2x2 square of cells with no internal walls that is the goal.
  auto goal = maze::Location(15, 15);
  for (uint8_t x = 0; x < config::kMazeWidth - 1; x++) {
    for (uint8_t y = 0; y < config::kMazeHeight - 1; y++) {
      auto a = maze::Read(maze::Location(x, y));
      auto b = maze::Read(maze::Location(x + 1, y + 1));
      if (a.visited && b.visited && !a.wall_east && !a.wall_north && !b.wall_west && !b.wall_south) {
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
  if (!maze::Read(goal + maze::Location(0, 1)).wall_north || !maze::Read(goal + maze::Location(0, 1)).wall_west) {
    goal += maze::Location(0, 1);
  } else if (!maze::Read(goal + maze::Location(1, 0)).wall_south
             || !maze::Read(goal + maze::Location(1, 0)).wall_east) {
    goal += maze::Location(1, 0);
  } else if (!maze::Read(goal + maze::Location(1, 1)).wall_north
             || !maze::Read(goal + maze::Location(1, 1)).wall_east) {
    goal += maze::Location(1, 1);
  }

  // Step 3. Floodfill outwards from the goal cell.
  stacks.Clear();
  auto goal_cell     = maze::Read(goal);
  goal_cell.distance = 0;
  maze::Write(goal, goal_cell);
  stacks.PushBack(goal);
  while (!stacks.Empty()) {
    auto loc  = stacks.PopFront();
    auto cell = maze::Read(loc);
    if (!cell.wall_north) {
      auto next      = loc + maze::Location(0, 1);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        stacks.PushBack(next);
      }
    }
    if (!cell.wall_east) {
      auto next      = loc + maze::Location(1, 0);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        stacks.PushBack(next);
      }
    }
    if (!cell.wall_south) {
      auto next      = loc - maze::Location(0, 1);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        stacks.PushBack(next);
      }
    }
    if (!cell.wall_west) {
      auto next      = loc - maze::Location(1, 0);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        stacks.PushBack(next);
      }
    }
  }
}

// solve follows the shortest path to the goal.
void Solve() {
  auto curr     = maze::Location(0, 0);
  auto distance = maze::Read(curr).distance;

  while (distance != 0) {
    auto north = curr + maze::Location(0, 1);
    auto east  = curr + maze::Location(1, 0);
    auto south = curr - maze::Location(0, 1);
    auto west  = curr - maze::Location(1, 0);

    auto next = curr;

    if (curr.Y() < (config::kMazeHeight - 1) && !maze::Read(curr).wall_north && maze::Read(north).distance < distance) {
      next     = north;
      distance = maze::Read(north).distance;
    }

    if (curr.X() < (config::kMazeWidth - 1) && !maze::Read(curr).wall_east && maze::Read(east).distance < distance) {
      next     = east;
      distance = maze::Read(east).distance;
    }

    if (curr.Y() > 0 && !maze::Read(curr).wall_south && maze::Read(south).distance < distance) {
      next     = south;
      distance = maze::Read(south).distance;
    }

    if (curr.X() > 0 && !maze::Read(curr).wall_west && maze::Read(west).distance < distance) {
      next     = west;
      distance = maze::Read(west).distance;
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
