#include "control.h"

#include "control/config.h"
#include "control/linear.h"
#include "control/plan.h"
#include "control/position.h"
#include "control/rotational.h"
#include "control/sensor_cal.h"
#include "control/speed.h"
#include "control/walls.h"
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
  walls_init();
  timer_set_callback(control_update);
}

void control_update() {
  pin_set(PROBE_TICK);
  speed_read();
  position_read();
  encoders_update();
  walls_update();

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

uint8_t control_report(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  static plan_state_t previous_plan_state = PLAN_STATE_SCHEDULED;
  static plan_type_t  previous_plan_type  = PLAN_TYPE_IDLE;
  static uint8_t      counter             = 0;

  // count how many ticks since the last plan change.
  if (current_plan.state == previous_plan_state && current_plan.type == previous_plan_type) {
    counter++;
  } else {
    counter             = 0;
    previous_plan_state = current_plan.state;
    previous_plan_type  = current_plan.type;
  }

  // if the report hasn't changed, only report every 8th tick.
  if (counter & 0x7) {
    return 0;
  }

  control_report_t *report = (control_report_t *)buffer;

  report->plan                 = current_plan;
  report->speed.measured_left  = speed_measured_left;
  report->speed.measured_right = speed_measured_right;
  report->speed.setpoint_left  = speed_setpoint_left;
  report->speed.setpoint_right = speed_setpoint_right;
  report->position.distance    = position_distance;
  report->position.theta       = position_theta;
  switch (current_plan.type) {
    case PLAN_TYPE_SENSOR_CAL:
      report->plan_data.sensor_cal.left   = sensor_threshold_left;
      report->plan_data.sensor_cal.right  = sensor_threshold_right;
      report->plan_data.sensor_cal.center = sensor_threshold_center;
      break;
    case PLAN_TYPE_ROTATIONAL_MOTION:
      report->plan_data.rotation.start_theta  = rotational_start_theta;
      report->plan_data.rotation.target_theta = rotational_target_theta;
      report->plan_data.rotation.direction    = rotational_direction;
      break;
    case PLAN_TYPE_LINEAR_MOTION:
      report->plan_data.linear.start_distance  = linear_start_distance;
      report->plan_data.linear.target_distance = linear_target_distance;
      report->plan_data.linear.target_speed    = linear_target_speed;
      break;
    default:
      break;
  }

  return sizeof(control_report_t);
}
