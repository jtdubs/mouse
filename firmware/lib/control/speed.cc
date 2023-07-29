#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "firmware/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/pid.hh"
#include "firmware/platform/platform.hh"
#include "speed_impl.hh"

namespace mouse::control::speed {

namespace {
// Motor speeds in RPMs.
float measured_left;
float measured_right;

// Motor speed setpointss in RPMs.
float setpoint_left;
float setpoint_right;

// PI controllers for the motors.
#if defined(ALLOW_SPEED_PID_TUNING)
pid::PIDController pid_left;
pid::PIDController pid_right;
float              alpha;
#else
pid::PIController pid_left;
pid::PIController pid_right;
#endif
}  // namespace

void Init() {
  pid_left.SetRange(-200, 200);
  pid_left.Tune(config::kSpeedKp, config::kSpeedKi, config::kSpeedKd);

  pid_right.SetRange(-200, 200);
  pid_right.Tune(config::kSpeedKp, config::kSpeedKi, config::kSpeedKd);

#if defined(ALLOW_SPEED_PID_TUNING)
  alpha = kSpeedAlpha;
#endif
}

void Update() {
  int32_t left_delta, right_delta;
  platform::encoders::ReadDeltas(left_delta, right_delta);

  float measured_left, measured_right;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    measured_left  = control::speed::measured_left;
    measured_right = control::speed::measured_right;
  }

#if defined(ALLOW_SPEED_PID_TUNING)
  measured_left = (alpha * config::CountsToRPM(left_delta))  //
                + ((1.0f - alpha) * measured_left);
  measured_right = (alpha * config::CountsToRPM(right_delta))  //
                 + ((1.0f - alpha) * measured_right);
#else
  measured_left = (config::kSpeedAlpha * config::CountsToRPM(left_delta))  //
                + ((1.0f - config::kSpeedAlpha) * measured_left);
  measured_right = (config::kSpeedAlpha * config::CountsToRPM(right_delta))  //
                 + ((1.0f - config::kSpeedAlpha) * measured_right);
#endif

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    control::speed::measured_left  = measured_left;
    control::speed::measured_right = measured_right;
  }
}

void Tick() {
  float measured_left, measured_right;
  float setpoint_left, setpoint_right;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    measured_left  = control::speed::measured_left;
    measured_right = control::speed::measured_right;
    setpoint_left  = control::speed::setpoint_left;
    setpoint_right = control::speed::setpoint_right;
  }

  float setpoint_left_mag = fabsf(setpoint_left);
  float power_left        = config::LeftRPMToPower(setpoint_left_mag);

  if (setpoint_left_mag < config::kMinMotorRPM) {
    pid_left.Reset();
    power_left = 0;
  } else {
    power_left += pid_left.Update(setpoint_left_mag, fabsf(measured_left));
  }

  float setpoint_right_mag = fabsf(setpoint_right);
  float power_right        = config::RightRPMToPower(setpoint_right_mag);

  if (setpoint_right_mag < config::kMinMotorRPM) {
    pid_right.Reset();
    power_right = 0;
  } else {
    power_right += pid_right.Update(setpoint_right_mag, fabsf(measured_right));
  }

  bool    forward_left  = signbitf(setpoint_left) == 0;
  bool    forward_right = signbitf(setpoint_right) == 0;
  int16_t left          = static_cast<int16_t>(lrintf(forward_left ? power_left : -power_left));
  int16_t right         = static_cast<int16_t>(lrintf(forward_right ? power_right : -power_right));
  platform::motor::Set(left, right);
}

void Set(float left, float right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    setpoint_left  = left;
    setpoint_right = right;
  }
}

void TunePID([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
             [[maybe_unused]] float alpha) {
#if defined(ALLOW_SPEED_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    alpha = alpha;

    pid_left.kp = kp;
    pid_left.ki = ki;

    pid_right.kp = kp;
    pid_right.ki = ki;
  }
#endif
}

// read reads the motor speeds.
void Read(float& left, float& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = measured_left;
    right = measured_right;
  }
}

// ReadSetpoints reads the motor speed setpoints.
void ReadSetpoints(float& left, float& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = setpoint_left;
    right = setpoint_right;
  }
}

}  // namespace mouse::control::speed
