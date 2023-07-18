#include <avr/interrupt.h>
#include <stddef.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "explore_impl.hh"
#include "firmware/lib/control/config.hh"
#include "firmware/lib/control/linear.hh"
#include "firmware/lib/control/plan.hh"
#include "firmware/lib/control/position.hh"
#include "firmware/lib/control/speed.hh"
#include "firmware/lib/control/walls.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/dequeue.hh"
#include "firmware/platform/platform.hh"

namespace explore {

namespace {
Orientation                             orientation;  // The current orientation of the mouse.
float                                   cell_offset;  // The offset into the current cell.
bool                                    stopped;      // Whether or not the mouse has stopped.
dequeue::Dequeue<maze::location_t, 256> path;         // The breadcrumb trail.
dequeue::Dequeue<maze::location_t, 256> next;         // The stack of unvisited cells.
dequeue::Dequeue<DequeueUpdate, 16>     updates;      // The queue of updates to send to the host.
}  // namespace

void explore() {
  path.RegisterCallback([](dequeue::Event event, maze::location_t value) {
    if (!updates.Full()) {
      updates.PushBack((DequeueUpdate){.dequeue_id = DequeueID::Path, .event = event, .value = value});
    }
  });
  next.RegisterCallback([](dequeue::Event event, maze::location_t value) {
    if (!updates.Full()) {
      updates.PushBack((DequeueUpdate){.dequeue_id = DequeueID::Next, .event = event, .value = value});
    }
  });

  // Idle the mouse and turn on the IR LEDs.
  plan::SubmitAndWait((plan::Plan){.type = plan::Type::Idle, .state = plan::State::Scheduled, .data = {.idle = {}}});
  plan::SubmitAndWait((plan::Plan){.type = plan::Type::IR, .state = plan::State::Scheduled, .data = {.ir = {true}}});

  // Assumption:
  // We start centered along the back wall of the starting square, with our back touching the wall.
  // Therefore our "position", measured by the center of the axle is kAxleOffset from the wall.
  orientation = Orientation::North;
  cell_offset = kAxleOffset;
  stopped     = true;

  // Our path so far is just the starting square, and we want to visit the square to our north.
  path.PushBack(maze::location(0, 0));
  next.PushBack(maze::location(0, 1));

  // While we have squares to visit...
  while (!next.Empty()) {
    // Skips cells we already visited since they were added to the stack.
    while (!next.Empty() && maze::Read(next.PeekBack()).visited) {
      next.PopBack();
    }

    // If we have no more cells to visit, then we are done.
    if (next.Empty()) {
      break;
    }

    auto curr_loc = path.PeekBack();
    auto next_loc = next.PeekBack();
    auto prev_loc = maze::location(0, 0);

    // Determine which direction to drive to reach the cell (if it is adjancent).
    Orientation next_orientation = adjacent(curr_loc, next_loc);

    // If we are adjacent to the next cell
    if (next_orientation != Orientation::Invalid) {
      next.PopBack();          // Remove the cell from the "next" stack.
      face(next_orientation);   // Turn to face the cell.
      advance(next_loc, true);  // Advance into it, updating the breadcrumb trail.
      classify(next_loc);       // Update our maze representation, and note any new cells to visit.
    } else {
      // Otherwise, backtrack a square.
      path.PopBack();                     // Remove the current cell from the breadcrumb trail.
      prev_loc = path.PeekBack();         // Check which cell we came from.
      face(adjacent(curr_loc, prev_loc));  // Turn to face it.
      advance(prev_loc, false);            // Advance into it, but do NOT update the breadcrumb trail.
    }
  }

  // Now that exploration is complete, we return to the starting cell.
  if (!path.Empty()) {
    auto curr = path.PopBack();
    while (!path.Empty()) {
      auto prev = path.PopBack();
      face(adjacent(curr, prev));
      advance(prev, false);
      curr = prev;
    }
  }

  // Stop in the middle of the last square, facing north.
  stop();
  face(Orientation::North);

  // Ensure the control system is idling (no motor activity).
  plan::SubmitAndWait((plan::Plan){.type = plan::Type::Idle, .state = plan::State::Scheduled, .data = {.idle = {}}});

  // Deregister dequeue callbacks.
  path.RegisterCallback(NULL);
  next.RegisterCallback(NULL);

  // Solve the maze, and tramit the new maze data to the host.
  floodfill();
  maze::Send();
}

// GetReport() is the report handler for the explore mode.
uint8_t GetReport(uint8_t *buffer, uint8_t len) {
  assert(assert::EXPLORE + 0, buffer != NULL);
  assert(assert::EXPLORE + 1, len >= (sizeof(DequeueUpdate) * 16));

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
Orientation adjacent(maze::location_t a, maze::location_t b) {
  auto ax = maze::x(a);
  auto ay = maze::y(a);
  auto bx = maze::x(b);
  auto by = maze::y(b);

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
void face(Orientation new_orientation) {
  assert(assert::EXPLORE + 2, new_orientation != Orientation::Invalid);

  if (new_orientation == orientation) {
    return;
  }

  // stop in the middle of the cell
  stop();

  // determine the delta between the current orientation and the desired orientation
  // Note: The (+4) and (&3) is to ensure the result is in [0,3] without using an expensive % operator.
  uint8_t delta  = (((uint8_t)new_orientation) + 4) - ((uint8_t)orientation);
  delta         &= 3;

  switch (delta) {
    case 0:
      break;
    case 1:
      plan::SubmitAndWait(  //
          (plan::Plan){.type  = plan::Type::RotationalMotion,
                       .state = plan::State::Scheduled,
                       .data  = {.rotational = {
                                     .d_theta = -M_PI_2,
                                }}});
      break;
    case 2:
      plan::SubmitAndWait(  //
          (plan::Plan){.type  = plan::Type::RotationalMotion,
                       .state = plan::State::Scheduled,
                       .data  = {.rotational = {
                                     .d_theta = M_PI,
                                }}});
      break;
    case 3:
      plan::SubmitAndWait(  //
          (plan::Plan){.type  = plan::Type::RotationalMotion,
                       .state = plan::State::Scheduled,
                       .data  = {.rotational = {
                                     .d_theta = M_PI_2,
                                }}});
      break;
  }

  float position, theta;
  position::Tare(position, theta);

  // assuming we were centered horizontally in the previous direction of travel
  // then we are now in the middle of the cell along the new direction of travel.
  cell_offset = kCellSize2;
  orientation = new_orientation;
}

// advance moves 1 square in the current direction, leaving the mouse
// in a position where the sensors are pointed at the centers of the side walls.
void advance(maze::location_t loc, bool update_path) {
  update_location();

  plan::SubmitAndWait(  //
      (plan::Plan){.type  = plan::Type::LinearMotion,
                   .state = plan::State::Scheduled,
                   .data  = {.linear = {
                                 .position = kCellSize - (cell_offset - ENTRY_OFFSET),
                                 .stop     = false,
                            }}});

  if (update_path) {
    path.PushBack(loc);
  }
  stopped = false;
}

// stop stops the mouse in the middle of the current cell.
void stop() {
  if (stopped) {
    return;
  }

  update_location();

  // we should never decide to stop once we have already passed the center of a cell.
  assert(assert::EXPLORE + 3, cell_offset <= kCellSize2);

  // stop at the center of the cell
  plan::SubmitAndWait(                                 //
      (plan::Plan){.type  = plan::Type::LinearMotion,  //
                   .state = plan::State::Scheduled,    //
                   .data  = {.linear = {
                                 .position = kCellSize2 - cell_offset,
                                 .stop     = true  //
                            }}});

  stopped = true;
}

// update_location updates the cell index and offset based on the traveled distance.
void update_location() {
  float position_distance, position_theta;
  position::Tare(position_distance, position_theta);
  cell_offset += position_distance;

  // this is a (% kCellSize) without using the % operator because it is expensive.
  while (cell_offset > kCellSize) {
    cell_offset -= kCellSize;
  }
}

// queue_unvisited adds a cell to the "next" stack if it has not already been visited.
void queue_unvisited(maze::location_t loc) {
  if (!maze::Read(loc).visited) {
    next.PushBack(loc);
  }
}

// classify updates the state of a cell, and adds unvisited neighbors to the "next" stack.
void classify(maze::location_t loc) {
  assert(assert::EXPLORE + 4, orientation != Orientation::Invalid);

  bool wall_forward, wall_left, wall_right;
  walls::Present(wall_left, wall_right, wall_forward);

  // Classify the square based on sensor readings.
  maze::cell_t cell = {
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
        queue_unvisited(loc + maze::location(1, 0));
      }
      if (wall_left) {
        cell.wall_west = wall_left;
      } else {
        queue_unvisited(loc - maze::location(1, 0));
      }
      if (wall_forward) {
        cell.wall_north = wall_forward;
      } else {
        queue_unvisited(loc + maze::location(0, 1));
      }
      break;
    case Orientation::East:
      if (wall_right) {
        cell.wall_south = wall_right;
      } else {
        queue_unvisited(loc - maze::location(0, 1));
      }
      if (wall_left) {
        cell.wall_north = wall_left;
      } else {
        queue_unvisited(loc + maze::location(0, 1));
      }
      if (wall_forward) {
        cell.wall_east = wall_forward;
      } else {
        queue_unvisited(loc + maze::location(1, 0));
      }
      break;
    case Orientation::South:
      if (wall_right) {
        cell.wall_west = wall_right;
      } else {
        queue_unvisited(loc - maze::location(1, 0));
      }
      if (wall_left) {
        cell.wall_east = wall_left;
      } else {
        queue_unvisited(loc + maze::location(1, 0));
      }
      if (wall_forward) {
        cell.wall_south = wall_forward;
      } else {
        queue_unvisited(loc - maze::location(0, 1));
      }
      break;
    case Orientation::West:
      if (wall_right) {
        cell.wall_north = wall_right;
      } else {
        queue_unvisited(loc + maze::location(0, 1));
      }
      if (wall_left) {
        cell.wall_south = wall_left;
      } else {
        queue_unvisited(loc - maze::location(0, 1));
      }
      if (wall_forward) {
        cell.wall_west = wall_forward;
      } else {
        queue_unvisited(loc - maze::location(1, 0));
      }
      break;
    case Orientation::Invalid:
      break;
  }

  maze::Write(loc, cell);
}

// floodfill calculates the shortest path to the goal.
void floodfill() {
  // Step 1. Find the 2x2 square of cells with no internal walls that is the goal.
  auto goal = maze::location(15, 15);
  for (uint8_t x = 0; x < kMazeWidth - 1; x++) {
    for (uint8_t y = 0; y < kMazeHeight - 1; y++) {
      auto a = maze::Read(maze::location(x, y));
      auto b = maze::Read(maze::location(x + 1, y + 1));
      if (a.visited && b.visited && !a.wall_east && !a.wall_north && !b.wall_west && !b.wall_south) {
        goal = maze::location(x, y);
        break;
      }
    }
  }
  if (goal == 0xFF) {
    // No goal found.
    return;
  }

  // Step 2. Find the cell in the goal square with the gateway, as that's where we want to get.
  if (!maze::Read(goal + maze::location(0, 1)).wall_north || !maze::Read(goal + maze::location(0, 1)).wall_west) {
    goal += maze::location(0, 1);
  } else if (!maze::Read(goal + maze::location(1, 0)).wall_south
             || !maze::Read(goal + maze::location(1, 0)).wall_east) {
    goal += maze::location(1, 0);
  } else if (!maze::Read(goal + maze::location(1, 1)).wall_north
             || !maze::Read(goal + maze::location(1, 1)).wall_east) {
    goal += maze::location(1, 1);
  }

  // Step 3. Floodfill outwards from the goal cell.
  path.Clear();
  auto goal_cell     = maze::Read(goal);
  goal_cell.distance = 0;
  maze::Write(goal, goal_cell);
  path.PushBack(goal);
  while (!path.Empty()) {
    auto loc  = path.PopFront();
    auto cell = maze::Read(loc);
    if (!cell.wall_north) {
      auto next      = loc + maze::location(0, 1);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        path.PushBack(next);
      }
    }
    if (!cell.wall_east) {
      auto next      = loc + maze::location(1, 0);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        path.PushBack(next);
      }
    }
    if (!cell.wall_south) {
      auto next      = loc - maze::location(0, 1);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        path.PushBack(next);
      }
    }
    if (!cell.wall_west) {
      auto next      = loc - maze::location(1, 0);
      auto next_cell = maze::Read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze::Write(next, next_cell);
        path.PushBack(next);
      }
    }
  }
}

// solve follows the shortest path to the goal.
void solve() {
  auto curr     = maze::location(0, 0);
  auto distance = maze::Read(curr).distance;

  while (distance != 0) {
    auto north = curr + maze::location(0, 1);
    auto east  = curr + maze::location(1, 0);
    auto south = curr - maze::location(0, 1);
    auto west  = curr - maze::location(1, 0);

    auto next = curr;

    if (maze::x(curr) < (kMazeHeight - 1) && !maze::Read(curr).wall_north && maze::Read(north).distance < distance) {
      next     = north;
      distance = maze::Read(north).distance;
    }

    if (maze::y(curr) < (kMazeWidth - 1) && !maze::Read(curr).wall_east && maze::Read(east).distance < distance) {
      next     = east;
      distance = maze::Read(east).distance;
    }

    if (maze::x(curr) > 0 && !maze::Read(curr).wall_south && maze::Read(south).distance < distance) {
      next     = south;
      distance = maze::Read(south).distance;
    }

    if (maze::y(curr) > 0 && !maze::Read(curr).wall_west && maze::Read(west).distance < distance) {
      next     = west;
      distance = maze::Read(west).distance;
    }

    if (next == curr) {
      // No path to the goal.
      return;
    }

    face(adjacent(curr, next));
    advance(next, false);
    curr = next;
  }

  // Stop in the middle of the goal cell.
  stop();

  // Ensure the control system is idling (no motor activity).
  plan::SubmitAndWait((plan::Plan){.type = plan::Type::Idle, .state = plan::State::Scheduled, .data = {.idle = {}}});
}

}  // namespace explore
