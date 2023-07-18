#include <stddef.h>

#include "config.hh"
#include "control_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/encoders.hh"
#include "firmware/platform/motor.hh"
#include "firmware/platform/pin.hh"
#include "firmware/platform/timer.hh"
#include "position_impl.hh"
#include "sensor_cal_impl.hh"
#include "speed_impl.hh"
#include "walls_impl.hh"

namespace control {

void init() {
  plan::init();
  speed::init();
  position::init();
  linear::init();
  rotational::init();
  sensor_cal::init();
  walls::init();
  timer::add_callback(tick);
}

void tick() {
  pin::set(pin::PROBE_TICK);
  speed::update();
  position::update();
  encoders::update();
  walls::update();

  plan::plan_t plan = plan::current();

  switch (plan.type) {
    case plan::TYPE_IDLE:
      if (plan.state == plan::STATE_SCHEDULED) {
        plan::set_state(plan::STATE_UNDERWAY);
        motor::set(0, 0);
        pin::clear(pin::LED_LEFT);
        pin::clear(pin::LED_RIGHT);
        pin::clear(pin::LED_ONBOARD);
        pin::clear(pin::IR_LEDS);
        plan::set_state(plan::STATE_IMPLEMENTED);
      }
      break;
    case plan::TYPE_LEDS:
      if (plan.state == plan::STATE_SCHEDULED) {
        plan::set_state(plan::STATE_UNDERWAY);
        pin::set2(pin::LED_LEFT, plan.data.leds.left);
        pin::set2(pin::LED_RIGHT, plan.data.leds.right);
        pin::set2(pin::LED_ONBOARD, plan.data.leds.onboard);
        plan::set_state(plan::STATE_IMPLEMENTED);
      }
      break;
    case plan::TYPE_IR:
      if (plan.state == plan::STATE_SCHEDULED) {
        plan::set_state(plan::STATE_UNDERWAY);
        pin::set2(pin::IR_LEDS, plan.data.ir.on);
        plan::set_state(plan::STATE_IMPLEMENTED);
      }
      break;
    case plan::TYPE_FIXED_POWER:
      if (plan.state == plan::STATE_SCHEDULED) {
        plan::set_state(plan::STATE_UNDERWAY);
        motor::set(plan.data.power.left, plan.data.power.right);
        plan::set_state(plan::STATE_IMPLEMENTED);
      }
      break;
    case plan::TYPE_FIXED_SPEED:
      if (plan.state == plan::STATE_SCHEDULED) {
        plan::set_state(plan::STATE_UNDERWAY);
        speed::set(plan.data.speed.left, plan.data.speed.right);
        plan::set_state(plan::STATE_IMPLEMENTED);
      }
      break;
    case plan::TYPE_LINEAR_MOTION:
      switch (plan.state) {
        case plan::STATE_SCHEDULED:
          linear::start(plan.data.linear.position, plan.data.linear.stop);
          plan::set_state(plan::STATE_UNDERWAY);
          [[fallthrough]];
        case plan::STATE_UNDERWAY:
          if (linear::tick()) {
            plan::set_state(plan::STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;
    case plan::TYPE_ROTATIONAL_MOTION:
      switch (plan.state) {
        case plan::STATE_SCHEDULED:
          rotational::start(plan.data.rotational.d_theta);
          plan::set_state(plan::STATE_UNDERWAY);
          [[fallthrough]];
        case plan::STATE_UNDERWAY:
          if (rotational::tick()) {
            plan::set_state(plan::STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;
    case plan::TYPE_SENSOR_CAL:
      switch (plan.state) {
        case plan::STATE_SCHEDULED:
          sensor_cal::start();
          plan::set_state(plan::STATE_UNDERWAY);
          break;
        case plan::STATE_UNDERWAY:
          if (sensor_cal::tick()) {
            plan::set_state(plan::STATE_IMPLEMENTED);
          }
          break;
        default:
          break;
      }
      break;
    default:
      // This should never happen.
      assert(assert::CONTROL + 0, false);
      break;
  }

  speed::tick();
  pin::clear(pin::PROBE_TICK);
}

uint8_t report(uint8_t *buffer, uint8_t len) {
  assert(assert::CONTROL + 1, buffer != NULL);
  assert(assert::CONTROL + 2, len >= sizeof(report_t));

  static plan::state_t previous_plan_state = plan::STATE_SCHEDULED;
  static plan::type_t  previous_plan_type  = plan::TYPE_IDLE;
  static uint8_t       counter             = 0;

  plan::plan_t plan = plan::current();

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

  report_t *report = (report_t *)buffer;

  report->plan = plan::current();
  speed::read(report->speed.measured_left, report->speed.measured_right);
  speed::read_setpoints(report->speed.setpoint_left, report->speed.setpoint_right);
  position::read(report->position.distance, report->position.theta);
  switch (report->plan.type) {
    case plan::TYPE_SENSOR_CAL:
      sensor_cal::read(report->plan_data.sensor_cal.left,   //
                       report->plan_data.sensor_cal.right,  //
                       report->plan_data.sensor_cal.forward);
      break;
    case plan::TYPE_ROTATIONAL_MOTION:
      rotational::read(report->plan_data.rotation);
      break;
    case plan::TYPE_LINEAR_MOTION:
      linear::read(report->plan_data.linear);
      break;
    default:
      break;
  }

  return sizeof(report_t);
}

}  // namespace control
