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
float measured_left_;
float measured_right_;

// Motor speed setpointss in RPMs.
float setpoint_left_;
float setpoint_right_;

// PI controllers for the motors.
#if defined(ALLOW_SPEED_PID_TUNING)
pid::PIDController pid_left_;
pid::PIDController pid_right_;
float              alpha_;
#else
pid::PIController pid_left_;
pid::PIController pid_right_;
#endif
}  // namespace

void Init() {
  pid_left_.SetRange(-200, 200);
  pid_left_.Tune(config::kSpeedKp, config::kSpeedKi, config::kSpeedKd);

  pid_right_.SetRange(-200, 200);
  pid_right_.Tune(config::kSpeedKp, config::kSpeedKi, config::kSpeedKd);

#if defined(ALLOW_SPEED_PID_TUNING)
  alpha_ = kSpeedAlpha;
#endif
}

void Update() {
  int32_t left_delta, right_delta;
  platform::encoders::ReadDeltas(left_delta, right_delta);

  float measured_left, measured_right;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    measured_left  = measured_left_;
    measured_right = measured_right_;
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
    measured_left_  = measured_left;
    measured_right_ = measured_right;
  }
}

void Tick() {
  float measured_left, measured_right;
  float setpoint_left, setpoint_right;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    measured_left  = measured_left_;
    measured_right = measured_right_;
    setpoint_left  = setpoint_left_;
    setpoint_right = setpoint_right_;
  }

  float setpoint_left_mag = fabsf(setpoint_left);
  float power_left        = config::LeftRPMToPower(setpoint_left_mag);

  if (setpoint_left_mag < config::kMinMotorRPM) {
    pid_left_.Reset();
    power_left = 0;
  } else {
    power_left += pid_left_.Update(setpoint_left_mag, fabsf(measured_left));
  }

  float setpoint_right_mag = fabsf(setpoint_right);
  float power_right        = config::RightRPMToPower(setpoint_right_mag);

  if (setpoint_right_mag < config::kMinMotorRPM) {
    pid_right_.Reset();
    power_right = 0;
  } else {
    power_right += pid_right_.Update(setpoint_right_mag, fabsf(measured_right));
  }

  bool    forward_left  = signbitf(setpoint_left) == 0;
  bool    forward_right = signbitf(setpoint_right) == 0;
  int16_t left          = static_cast<int16_t>(lrintf(forward_left ? power_left : -power_left));
  int16_t right         = static_cast<int16_t>(lrintf(forward_right ? power_right : -power_right));
  platform::motor::Set(left, right);
}

void Set(float left, float right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    setpoint_left_  = left;
    setpoint_right_ = right;
  }
}

void TunePID([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
             [[maybe_unused]] float alpha) {
#if defined(ALLOW_SPEED_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    alpha = alpha;

    pid_left_.kp = kp;
    pid_left_.ki = ki;

    pid_right_.kp = kp;
    pid_right_.ki = ki;
  }
#endif
}

// read reads the motor speeds.
void Read(float& left, float& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = measured_left_;
    right = measured_right_;
  }
}

// ReadSetpoints reads the motor speed setpoints.
void ReadSetpoints(float& left, float& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = setpoint_left_;
    right = setpoint_right_;
  }
}

}  // namespace mouse::control::speed
