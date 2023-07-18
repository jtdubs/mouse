#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/sim.h"
#include "firmware/platform/platform.hh"
#include "plan_impl.hh"

namespace plan {

namespace {
// current_plan is the current plan
Plan current_plan;
}  // namespace

// Init initializes the plan module.
void Init() {
  current_plan.type = Type::Idle;
  SetState(State::Scheduled);
}

// submit submits a new plan to be implemented.
void Submit(Plan plan) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    current_plan = plan;
    SetState(State::Scheduled);
  }
}

// wait waits for the current plan to be implemented.
void Wait() {
  pin::Clear(pin::kProbePlan);
  volatile State *state = &current_plan.state;
  while (*state != State::Implemented) {}
  pin::Set(pin::kProbePlan);
}

// SubmitAndWait submits a new plan, and wait for it to be implemented.
// NOTE: this plan will replace the current plan, even if it is not fully implemented!
void SubmitAndWait(Plan plan) {
  Submit(plan);
  Wait();
}

// SetState sets the current plan state.
void SetState(State state) {
  current_plan.state = state;
  sim_watch_plan((uint8_t)state);
}

// current gets the current plan.
Plan Current() {
  Plan result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = current_plan;
  }
  return result;
}

}  // namespace plan
