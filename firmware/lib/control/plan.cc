#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/sim.h"
#include "firmware/platform/pin.hh"
#include "plan_impl.hh"

namespace plan {

namespace {
// current_plan is the current plan
plan_t current_plan;
}  // namespace

// init initializes the plan module.
void init() {
  current_plan.type = TYPE_IDLE;
  set_state(STATE_SCHEDULED);
}

// submit submits a new plan to be implemented.
void submit(plan_t plan) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    current_plan = plan;
    set_state(STATE_SCHEDULED);
  }
}

// wait waits for the current plan to be implemented.
void wait() {
  pin::clear(pin::PROBE_PLAN);
  volatile state_t *state = &current_plan.state;
  while (*state != STATE_IMPLEMENTED) {}
  pin::set(pin::PROBE_PLAN);
}

// submit_and_wait submits a new plan, and wait for it to be implemented.
// NOTE: this plan will replace the current plan, even if it is not fully implemented!
void submit_and_wait(plan_t plan) {
  submit(plan);
  wait();
}

// set_state sets the current plan state.
void set_state(state_t state) {
  current_plan.state = state;
  sim_watch_plan(state);
}

// read reads the current plan.
void read(plan_t *plan) {
  assert(assert::PLAN + 0, plan != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *plan = current_plan;
  }
}

}  // namespace plan