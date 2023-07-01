#include "control.h"

#include "control/linear.h"
#include "control/plan.h"
#include "control/position.h"
#include "control/speed.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/timer.h"
#include "serial/report.h"

void control_init() {
  plan_init();
  speed_init();
  position_init();
  timer_set_callback(control_update);
}

void control_update() {
  pin_set(PROBE_TICK);
  speed_read();
  position_read();
  encoders_update();

  switch (current_plan.type) {
    case PLAN_TYPE_IDLE:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        motor_set(0, 0);
        plan_set_state(PLAN_STATE_UNDERWAY);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_FIXED_POWER:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        motor_set(current_plan.data.power.left, current_plan.data.power.right);
        plan_set_state(PLAN_STATE_UNDERWAY);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_FIXED_SPEED:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        speed_set(current_plan.data.speed.left, current_plan.data.speed.right);
        plan_set_state(PLAN_STATE_UNDERWAY);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      speed_update();
      break;
    case PLAN_TYPE_LINEAR_MOTION:
      switch (current_plan.state) {
        case PLAN_STATE_SCHEDULED:
          linear_start(current_plan.data.linear.distance, current_plan.data.linear.exit_speed, 9810.0 * 0.2 /* 0.2g */);
          plan_set_state(PLAN_STATE_UNDERWAY);
          [[fallthrough]];
        case PLAN_STATE_UNDERWAY:
          if (linear_update()) {
            plan_set_state(PLAN_STATE_IMPLEMENTED);
          }
          break;
        default:
          speed_update();
          break;
      }
      break;
    case PLAN_TYPE_ROTATIONAL_MOTION:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    default:
      break;
  }

  report_send();
  pin_clear(PROBE_TICK);
}
