#include "control.h"

#include "control/plan.h"
#include "control/speed.h"
#include "platform/motor.h"
#include "platform/timer.h"
#include "serial/report.h"

void control_init() {
  plan_init();
  speed_init();
  timer_set_callback(control_update);
}

void control_update() {
  pin_set(PROBE_TICK);
  speed_read();

  switch (current_plan.type) {
    case PLAN_IDLE:
      if (!current_plan.implemented) {
        motor_set(0, 0);
        current_plan.implemented = true;
      }
      break;
    case PLAN_FIXED_POWER:
      if (!current_plan.implemented) {
        motor_set(current_plan.data.power.left, current_plan.data.power.right);
        current_plan.implemented = true;
      }
      break;
    case PLAN_FIXED_SPEED:
      if (!current_plan.implemented) {
        speed_set(current_plan.data.speed.left, current_plan.data.speed.right);
        current_plan.implemented = true;
      }
      speed_update();
      break;
    case PLAN_LINEAR_MOTION:
      if (!current_plan.implemented) {
        current_plan.implemented = true;
      }
      break;
    case PLAN_ROTATIONAL_MOTION:
      if (!current_plan.implemented) {
        current_plan.implemented = true;
      }
      break;
    default:
      break;
  }

  report_send();
  pin_clear(PROBE_TICK);
}
