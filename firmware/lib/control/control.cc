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

void Init() {
  plan::Init();
  speed::Init();
  position::Init();
  linear::Init();
  rotational::Init();
  sensor_cal::Init();
  walls::Init();
  timer::AddCallback(Tick);
}

void Tick() {
  pin::Set(pin::kProbeTick);
  speed::Update();
  position::Update();
  encoders::Update();
  walls::Update();

  auto plan = plan::Current();

  switch (plan.type) {
    case plan::Type::Idle:
      if (plan.state == plan::State::Scheduled) {
        plan::SetState(plan::State::Underway);
        motor::Set(0, 0);
        pin::Clear(pin::kLEDLeft);
        pin::Clear(pin::kLEDRight);
        pin::Clear(pin::kLEDOnboard);
        pin::Clear(pin::IR_LEDS);
        plan::SetState(plan::State::Implemented);
      }
      break;
    case plan::Type::LEDs:
      if (plan.state == plan::State::Scheduled) {
        plan::SetState(plan::State::Underway);
        pin::Set2(pin::kLEDLeft, plan.data.leds.left);
        pin::Set2(pin::kLEDRight, plan.data.leds.right);
        pin::Set2(pin::kLEDOnboard, plan.data.leds.onboard);
        plan::SetState(plan::State::Implemented);
      }
      break;
    case plan::Type::IR:
      if (plan.state == plan::State::Scheduled) {
        plan::SetState(plan::State::Underway);
        pin::Set2(pin::IR_LEDS, plan.data.ir.on);
        plan::SetState(plan::State::Implemented);
      }
      break;
    case plan::Type::FixedPower:
      if (plan.state == plan::State::Scheduled) {
        plan::SetState(plan::State::Underway);
        motor::Set(plan.data.power.left, plan.data.power.right);
        plan::SetState(plan::State::Implemented);
      }
      break;
    case plan::Type::FixedSpeed:
      if (plan.state == plan::State::Scheduled) {
        plan::SetState(plan::State::Underway);
        speed::Set(plan.data.speed.left, plan.data.speed.right);
        plan::SetState(plan::State::Implemented);
      }
      break;
    case plan::Type::LinearMotion:
      switch (plan.state) {
        case plan::State::Scheduled:
          linear::Start(plan.data.linear.position, plan.data.linear.stop);
          plan::SetState(plan::State::Underway);
          [[fallthrough]];
        case plan::State::Underway:
          if (linear::Tick()) {
            plan::SetState(plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    case plan::Type::RotationalMotion:
      switch (plan.state) {
        case plan::State::Scheduled:
          rotational::Start(plan.data.rotational.d_theta);
          plan::SetState(plan::State::Underway);
          [[fallthrough]];
        case plan::State::Underway:
          if (rotational::Tick()) {
            plan::SetState(plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    case plan::Type::SensorCal:
      switch (plan.state) {
        case plan::State::Scheduled:
          sensor_cal::Start();
          plan::SetState(plan::State::Underway);
          break;
        case plan::State::Underway:
          if (sensor_cal::Tick()) {
            plan::SetState(plan::State::Implemented);
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

  speed::Tick();
  pin::Clear(pin::kProbeTick);
}

uint8_t GetReport(uint8_t *buffer, uint8_t len) {
  assert(assert::CONTROL + 1, buffer != NULL);
  assert(assert::CONTROL + 2, len >= sizeof(Report));

  static auto    previous_plan_state = plan::State::Scheduled;
  static auto    previous_Planype    = plan::Type::Idle;
  static uint8_t counter             = 0;

  auto plan = plan::Current();

  // count how many ticks since the last plan change.
  if (plan.state == previous_plan_state && plan.type == previous_Planype) {
    counter++;
  } else {
    counter             = 0;
    previous_plan_state = plan.state;
    previous_Planype    = plan.type;
  }

  // if the report hasn't changed, only report every 8th Tick.
  if (counter & 0x7) {
    return 0;
  }

  auto *report = (Report *)buffer;

  report->plan = plan::Current();
  speed::Read(report->speed.measured_left, report->speed.measured_right);
  speed::ReadSetpoints(report->speed.setpoint_left, report->speed.setpoint_right);
  position::Read(report->position.distance, report->position.theta);
  switch (report->plan.type) {
    case plan::Type::SensorCal:
      sensor_cal::Read(report->plan_data.sensor_cal.left,   //
                       report->plan_data.sensor_cal.right,  //
                       report->plan_data.sensor_cal.forward);
      break;
    case plan::Type::RotationalMotion:
      rotational::Read(report->plan_data.rotation);
      break;
    case plan::Type::LinearMotion:
      linear::Read(report->plan_data.linear);
      break;
    default:
      break;
  }

  return sizeof(Report);
}

}  // namespace control
