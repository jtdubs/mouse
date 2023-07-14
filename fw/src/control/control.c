#include "control.h"

#include <stddef.h>

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
#include "utils/assert.h"

#pragma pack(push, 1)
// control_report_t is the report sent by the control module.
typedef struct {
  plan_t plan;
  struct {
    float measured_left;   // RPMs
    float measured_right;  // RPMs
    float setpoint_left;   // RPMs
    float setpoint_right;  // RPMs
  } speed;
  struct {
    float distance;  // mm
    float theta;     // radians
  } position;
  union {
    struct {
      uint16_t left;    // ADC reading
      uint16_t right;   // ADC reading
      uint16_t center;  // ADC reading
    } sensor_cal;
    rotational_state_t rotation;
    linear_state_t     linear;
  } plan_data;
} control_report_t;
#pragma pack(pop)

void control_init() {
  plan_init();
  speed_init();
  position_init();
  linear_init();
  rotational_init();
  sensor_cal_init();
  walls_init();
  timer_set_callback(control_tick);
}

void control_tick() {
  pin_set(PROBE_TICK);
  speed_update();
  position_update();
  encoders_update();
  walls_update();

  plan_t plan;
  plan_read(&plan);

  switch (plan.type) {
    case PLAN_TYPE_IDLE:
      if (plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        motor_set(0, 0);
        pin_clear(LED_LEFT);
        pin_clear(LED_RIGHT);
        pin_clear(LED_BUILTIN);
        pin_clear(IR_LEDS);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_LEDS:
      if (plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        pin_set2(LED_LEFT, plan.data.leds.left);
        pin_set2(LED_RIGHT, plan.data.leds.right);
        pin_set2(LED_BUILTIN, plan.data.leds.builtin);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_IR:
      if (plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        pin_set2(IR_LEDS, plan.data.ir.on);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_FIXED_POWER:
      if (plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        motor_set(plan.data.power.left, plan.data.power.right);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_FIXED_SPEED:
      if (plan.state == PLAN_STATE_SCHEDULED) {
        plan_set_state(PLAN_STATE_UNDERWAY);
        speed_set(plan.data.speed.left, plan.data.speed.right);
        plan_set_state(PLAN_STATE_IMPLEMENTED);
      }
      break;
    case PLAN_TYPE_LINEAR_MOTION:
      switch (plan.state) {
        case PLAN_STATE_SCHEDULED:
          linear_start(plan.data.linear.distance, plan.data.linear.stop);
          plan_set_state(PLAN_STATE_UNDERWAY);
          [[fallthrough]];
        case PLAN_STATE_UNDERWAY:
          if (linear_tick()) {
            plan_set_state(PLAN_STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;
    case PLAN_TYPE_ROTATIONAL_MOTION:
      switch (plan.state) {
        case PLAN_STATE_SCHEDULED:
          rotational_start(plan.data.rotational.d_theta);
          plan_set_state(PLAN_STATE_UNDERWAY);
          [[fallthrough]];
        case PLAN_STATE_UNDERWAY:
          if (rotational_tick()) {
            plan_set_state(PLAN_STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;
    case PLAN_TYPE_SENSOR_CAL:
      switch (plan.state) {
        case PLAN_STATE_SCHEDULED:
          sensor_cal_start();
          plan_set_state(PLAN_STATE_UNDERWAY);
          break;
        case PLAN_STATE_UNDERWAY:
          if (sensor_cal_tick()) {
            plan_set_state(PLAN_STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;
    default:
      // This should never happen.
      assert(ASSERT_CONTROL + 0, false);
      break;
  }

  speed_tick();
  report_send();
  pin_clear(PROBE_TICK);
}

uint8_t control_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_CONTROL + 1, buffer != NULL);
  assert(ASSERT_CONTROL + 2, len >= sizeof(control_report_t));

  static plan_state_t previous_plan_state = PLAN_STATE_SCHEDULED;
  static plan_type_t  previous_plan_type  = PLAN_TYPE_IDLE;
  static uint8_t      counter             = 0;

  plan_t plan;
  plan_read(&plan);

  // count how many ticks since the last plan change.
  if (plan.state == previous_plan_state && plan.type == previous_plan_type) {
    counter++;
  } else {
    counter             = 0;
    previous_plan_state = plan.state;
    previous_plan_type  = plan.type;
  }

  // if the report hasn't changed, only report every 8th tick.
  if (counter & 0x7) {
    return 0;
  }

  control_report_t *report = (control_report_t *)buffer;

  plan_read(&report->plan);
  speed_read(&report->speed.measured_left, &report->speed.measured_right);
  speed_read_setpoints(&report->speed.setpoint_left, &report->speed.setpoint_right);
  position_read(&report->position.distance, &report->position.theta);
  switch (report->plan.type) {
    case PLAN_TYPE_SENSOR_CAL:
      sensor_cal_read(&report->plan_data.sensor_cal.left,   //
                      &report->plan_data.sensor_cal.right,  //
                      &report->plan_data.sensor_cal.center);
      break;
    case PLAN_TYPE_ROTATIONAL_MOTION:
      rotational_state(&report->plan_data.rotation);
      break;
    case PLAN_TYPE_LINEAR_MOTION:
      linear_state(&report->plan_data.linear);
      break;
    default:
      break;
  }

  return sizeof(control_report_t);
}
