#include <stddef.h>

#include "control_impl.hh"
#include "firmware/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"
#include "linear_impl.hh"
#include "plan_impl.hh"
#include "position_impl.hh"
#include "rotational_impl.hh"
#include "sensor_cal_impl.hh"
#include "speed_impl.hh"
#include "walls_impl.hh"

namespace mouse::control {

void Init() {
  control::plan::Init();
  control::speed::Init();
  position::Init();
  control::linear::Init();
  rotational::Init();
  sensor_cal::Init();
  walls::Init();
  platform::timer::AddCallback(Tick);
}

void Tick() {
  platform::pin::Set(platform::pin::kProbeTick);
  control::speed::Update();
  position::Update();
  platform::encoders::Update();
  walls::Update();

  auto plan = control::plan::Current();

  switch (plan.type) {
    case control::plan::Type::Idle:
      if (plan.state == control::plan::State::Scheduled) {
        control::plan::SetState(control::plan::State::Underway);
        platform::motor::Set(0, 0);
        platform::pin::Clear(platform::pin::kLEDLeft);
        platform::pin::Clear(platform::pin::kLEDRight);
        platform::pin::Clear(platform::pin::kLEDOnboard);
        platform::pin::Clear(platform::pin::kIRLEDs);
        control::plan::SetState(control::plan::State::Implemented);
      }
      break;
    case control::plan::Type::LEDs:
      if (plan.state == control::plan::State::Scheduled) {
        control::plan::SetState(control::plan::State::Underway);
        platform::pin::Set(platform::pin::kLEDLeft, plan.data.leds.left);
        platform::pin::Set(platform::pin::kLEDRight, plan.data.leds.right);
        platform::pin::Set(platform::pin::kLEDOnboard, plan.data.leds.onboard);
        control::plan::SetState(control::plan::State::Implemented);
      }
      break;
    case control::plan::Type::IR:
      if (plan.state == control::plan::State::Scheduled) {
        control::plan::SetState(control::plan::State::Underway);
        platform::pin::Set(platform::pin::kIRLEDs, plan.data.ir.on);
        control::plan::SetState(control::plan::State::Implemented);
      }
      break;
    case control::plan::Type::FixedPower:
      if (plan.state == control::plan::State::Scheduled) {
        control::plan::SetState(control::plan::State::Underway);
        platform::motor::Set(plan.data.power.left, plan.data.power.right);
        control::plan::SetState(control::plan::State::Implemented);
      }
      break;
    case control::plan::Type::FixedSpeed:
      if (plan.state == control::plan::State::Scheduled) {
        control::plan::SetState(control::plan::State::Underway);
        control::speed::Set(plan.data.speed.left, plan.data.speed.right);
        control::plan::SetState(control::plan::State::Implemented);
      }
      break;
    case control::plan::Type::LinearMotion:
      switch (plan.state) {
        case control::plan::State::Scheduled:
          control::linear::Start(plan.data.linear.position, plan.data.linear.stop);
          control::plan::SetState(control::plan::State::Underway);
          [[fallthrough]];
        case control::plan::State::Underway:
          if (control::linear::Tick()) {
            control::plan::SetState(control::plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    case control::plan::Type::RotationalMotion:
      switch (plan.state) {
        case control::plan::State::Scheduled:
          rotational::Start(plan.data.rotational.d_theta);
          control::plan::SetState(control::plan::State::Underway);
          [[fallthrough]];
        case control::plan::State::Underway:
          if (rotational::Tick()) {
            control::plan::SetState(control::plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    case control::plan::Type::SensorCal:
      switch (plan.state) {
        case control::plan::State::Scheduled:
          sensor_cal::Start();
          control::plan::SetState(control::plan::State::Underway);
          break;
        case control::plan::State::Underway:
          if (sensor_cal::Tick()) {
            control::plan::SetState(control::plan::State::Implemented);
          }
          break;
        default:
          break;
      }
      break;
    default:
      // This should never happen.
      assert(assert::Module::Control, 0, false);
      break;
  }

  control::speed::Tick();
  platform::pin::Clear(platform::pin::kProbeTick);
}

uint8_t GetReport(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  assert(assert::Module::Control, 1, buffer != NULL);
  assert(assert::Module::Control, 2, len >= sizeof(Report));

  static auto    previous_plan_state = control::plan::State::Scheduled;
  static auto    previous_plan_type  = control::plan::Type::Idle;
  static uint8_t counter             = 0;

  auto plan = control::plan::Current();

  // count how many ticks since the last plan change.
  if (plan.state == previous_plan_state && plan.type == previous_plan_type) {
    counter++;
  } else {
    counter             = 0;
    previous_plan_state = plan.state;
    previous_plan_type  = plan.type;
  }

  // if the report hasn't changed, only report every 8th Tick.
  if (counter & 0x7) {
    return 0;
  }

  auto *report = (Report *)buffer;

  report->plan = control::plan::Current();
  control::speed::Read(report->speed.measured_left, report->speed.measured_right);
  control::speed::ReadSetpoints(report->speed.setpoint_left, report->speed.setpoint_right);
  position::Read(report->position.distance, report->position.theta);
  switch (report->plan.type) {
    case control::plan::Type::SensorCal:
      sensor_cal::Read(report->plan_data.sensor_cal.left,   //
                       report->plan_data.sensor_cal.right,  //
                       report->plan_data.sensor_cal.forward);
      break;
    case control::plan::Type::RotationalMotion:
      rotational::Read(report->plan_data.rotation);
      break;
    case control::plan::Type::LinearMotion:
      control::linear::Read(report->plan_data.linear);
      break;
    default:
      break;
  }

  return sizeof(Report);
}

}  // namespace mouse::control
