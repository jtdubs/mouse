#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/sim.h"
#include "firmware/platform/platform.hh"
#include "plan_impl.hh"

namespace mouse::control::plan {

namespace {
// current_plan is the current plan
Plan plan_;
}  // namespace

// Init initializes the plan module.
void Init() {
  plan_.type = Type::Idle;
  SetState(State::Scheduled);
}

// submit submits a new plan to be implemented.
void Submit(Plan plan) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    plan_ = plan;
    SetState(State::Scheduled);
  }
}

// wait waits for the current plan to be implemented.
void Wait() {
  platform::probe::Clear(platform::probe::Probe::Plan);
  volatile State *state = &plan_.state;
  while (*state != State::Implemented) {}
  platform::probe::Set(platform::probe::Probe::Plan);
}

// SubmitAndWait submits a new plan, and wait for it to be implemented.
// NOTE: this plan will replace the current plan, even if it is not fully implemented!
void SubmitAndWait(Plan plan) {
  Submit(plan);
  Wait();
}

// SetState sets the current plan state.
void SetState(State state) {
  plan_.state = state;
  sim_watch_plan(static_cast<uint8_t>(state));
}

// current gets the current plan.
Plan Current() {
  Plan result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = plan_;
  }
  return result;
}

}  // namespace mouse::control::plan
