#include "control.h"

#include "control/config.h"
#include "control/linear.h"
#include "control/plan.h"
#include "control/position.h"
#include "control/rotational.h"
#include "control/sensor_cal.h"
#include "control/speed.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/timer.h"
#include "serial/report.h"

void control_init() {
  plan_init();
  speed_init();
  position_init();
  linear_init();
  rotational_init();
  sensor_cal_init();
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
        plan_set_state(PLAN_STATE_UNDERWAY);
        motor_set(0, 0);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_LEDS:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        pin_set2(LED_LEFT, current_plan.data.leds.left);
        pin_set2(LED_RIGHT, current_plan.data.leds.right);
        pin_set2(LED_BUILTIN, current_plan.data.leds.builtin);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_IR:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        pin_set2(IR_LEDS, current_plan.data.ir.on);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;

    case PLAN_TYPE_FIXED_POWER:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        motor_set(current_plan.data.power.left, current_plan.data.power.right);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_FIXED_SPEED:
      if (current_plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        speed_set(current_plan.data.speed.left, current_plan.data.speed.right);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      speed_update();
      break;
    case PLAN_TYPE_LINEAR_MOTION:
      switch (current_plan.state) {
        case PLAN_STATE_SCHEDULED:
          linear_start(current_plan.data.linear.distance, current_plan.data.linear.stop);
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
      switch (current_plan.state) {
        case PLAN_STATE_SCHEDULED:
          rotational_start(current_plan.data.rotational.d_theta);
          plan_set_state(PLAN_STATE_UNDERWAY);
          [[fallthrough]];
        case PLAN_STATE_UNDERWAY:
          if (rotational_update()) {
            plan_set_state(PLAN_STATE_IMPLEMENTED);
          }
          break;
        default:
          speed_update();
          break;
      }
      break;
    case PLAN_TYPE_SENSOR_CAL:
      switch (current_plan.state) {
        case PLAN_STATE_SCHEDULED:
          sensor_cal_start();
          plan_set_state(PLAN_STATE_UNDERWAY);
          break;
        case PLAN_STATE_UNDERWAY:
          if (sensor_cal_update()) {
            plan_set_state(PLAN_STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;

    default:
      break;
  }

  report_send();
  pin_clear(PROBE_TICK);
}
