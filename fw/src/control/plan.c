#include "plan.h"

#include <util/atomic.h>

#include "utils/assert.h"

// current_plan is the current plan
volatile plan_t current_plan;

// plan_init initializes the plan module.
void plan_init() {
  current_plan.type  = PLAN_IDLE;
  current_plan.state = PLAN_STATE_SCHEDULED;
}

// plan_submit submits a new plan to be implemented.
void plan_submit(plan_t *plan) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    assert(ASSERT_PLAN + 0, plan->state == PLAN_STATE_SCHEDULED);
    current_plan = *plan;
  }
}

// plan_wait waits for the current plan to be implemented.
void plan_wait() {
  while (current_plan.state != PLAN_STATE_IMPLEMENTED) {
    // wait
  }
}

// plan_submit_and_wait submits a new plan, and wait for it to be implemented.
// NOTE: this plan will replace the current plan, even if it is not fully implemented!
void plan_submit_and_wait(plan_t *plan) {
  plan_submit(plan);
  plan_wait();
}
