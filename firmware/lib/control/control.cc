#include <stddef.h>

#include "config.hh"
#include "control_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"
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

  auto plan = plan::current();

  switch (plan.type) {
    case plan::Type::Idle:
      if (plan.state == plan::State::Scheduled) {
        plan::set_state(plan::State::Underway);
        motor::set(0, 0);
        pin::clear(pin::LED_LEFT);
        pin::clear(pin::LED_RIGHT);
        pin::clear(pin::LED_ONBOARD);
        pin::clear(pin::IR_LEDS);
        plan::set_state(plan::State::Implemented);
      }
      break;
    case plan::Type::LEDs:
      if (plan.state == plan::State::Scheduled) {
        plan::set_state(plan::State::Underway);
        pin::set2(pin::LED_LEFT, plan.data.leds.left);
        pin::set2(pin::LED_RIGHT, plan.data.leds.right);
        pin::set2(pin::LED_ONBOARD, plan.data.leds.onboard);
        plan::set_state(plan::State::Implemented);
      }
      break;
    case plan::Type::IR:
      if (plan.state == plan::State::Scheduled) {
        plan::set_state(plan::State::Underway);
        pin::set2(pin::IR_LEDS, plan.data.ir.on);
        plan::set_state(plan::State::Implemented);
      }
      break;
    case plan::Type::FixedPower:
      if (plan.state == plan::State::Scheduled) {
        plan::set_state(plan::State::Underway);
        motor::set(plan.data.power.left, plan.data.power.right);
        plan::set_state(plan::State::Implemented);
      }
      break;
    case plan::Type::FixedSpeed:
      if (plan.state == plan::State::Scheduled) {
        plan::set_state(plan::State::Underway);
        speed::set(plan.data.speed.left, plan.data.speed.right);
        plan::set_state(plan::State::Implemented);
      }
      break;
    case plan::Type::LinearMotion:
      switch (plan.state) {
        case plan::State::Scheduled:
          linear::start(plan.data.linear.position, plan.data.linear.stop);
          plan::set_state(plan::State::Underway);
          [[fallthrough]];
        case plan::State::Underway:
          if (linear::tick()) {
            plan::set_state(plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    case plan::Type::RotationalMotion:
      switch (plan.state) {
        case plan::State::Scheduled:
          rotational::start(plan.data.rotational.d_theta);
          plan::set_state(plan::State::Underway);
          [[fallthrough]];
        case plan::State::Underway:
          if (rotational::tick()) {
            plan::set_state(plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    case plan::Type::SensorCal:
      switch (plan.state) {
        case plan::State::Scheduled:
          sensor_cal::start();
          plan::set_state(plan::State::Underway);
          break;
        case plan::State::Underway:
          if (sensor_cal::tick()) {
            plan::set_state(plan::State::Implemented);
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
  assert(assert::CONTROL + 2, len >= sizeof(Report));

  static auto    previous_plan_state = plan::State::Scheduled;
  static auto    previous_Planype    = plan::Type::Idle;
  static uint8_t counter             = 0;

  auto plan = plan::current();

  // count how many ticks since the last plan change.
  if (plan.state == previous_plan_state && plan.type == previous_Planype) {
    counter++;
  } else {
    counter             = 0;
    previous_plan_state = plan.state;
    previous_Planype    = plan.type;
  }

  // if the report hasn't changed, only report every 8th tick.
  if (counter & 0x7) {
    return 0;
  }

  auto *report = (Report *)buffer;

  report->plan = plan::current();
  speed::read(report->speed.measured_left, report->speed.measured_right);
  speed::read_setpoints(report->speed.setpoint_left, report->speed.setpoint_right);
  position::read(report->position.distance, report->position.theta);
  switch (report->plan.type) {
    case plan::Type::SensorCal:
      sensor_cal::read(report->plan_data.sensor_cal.left,   //
                       report->plan_data.sensor_cal.right,  //
                       report->plan_data.sensor_cal.forward);
      break;
    case plan::Type::RotationalMotion:
      rotational::read(report->plan_data.rotation);
      break;
    case plan::Type::LinearMotion:
      linear::read(report->plan_data.linear);
      break;
    default:
      break;
  }

  return sizeof(Report);
}

}  // namespace control
