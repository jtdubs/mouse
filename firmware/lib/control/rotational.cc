#include <stdbool.h>
#include <util/atomic.h>

#include "firmware/config.hh"
#include "firmware/lib/utils/math.hh"
#include "position_impl.hh"
#include "rotational_impl.hh"
#include "speed_impl.hh"

namespace mouse::control::rotational {

namespace {
State state_;
}  // namespace

void Init() {}

void Start(float dtheta /* radians */) {
  float distance, theta;
  position::Read(distance, theta);

  State s = {
      .start_theta  = theta,
      .target_theta = theta + dtheta,
      .direction    = dtheta > 0,
  };

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state_ = s;
  }
}

bool Tick() {
  float speed_measured_left, speed_measured_right;
  control::speed::Read(speed_measured_left, speed_measured_right);

  float speed_setpoint_left, speed_setpoint_right;
  control::speed::ReadSetpoints(speed_setpoint_left, speed_setpoint_right);

  float position_distance, position_theta;
  position::Read(position_distance, position_theta);

  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state_;
  }

  float dtheta = s.target_theta - position_theta;
  s.direction  = dtheta > 0;

  // If we are in the right range...
  if (fabsf(dtheta) <= (config::kCountTheta * 12.0)) {
    // Stop the motors.
    control::speed::Set(0, 0);
    // We are done when the measured speed < 0.1mm/s
    return (fabsf(config::RPMToSpeed(speed_measured_left)) < 0.1) &&  //
           (fabsf(config::RPMToSpeed(speed_measured_right)) < 0.1);
  }

  float current_setpoint = config::RPMToSpeed(fabsf(speed_setpoint_left));  // mm/s

  // Compute the braking distance.
  float dV            = -current_setpoint;                                                     // mm/s
  float dX            = fabsf(s.target_theta - position_theta) * (config::kWheelBase / 2.0f);  // mm
  float braking_accel = ((2.0f * current_setpoint * dV) + (dV * dV)) / (2.0f * dX);            // mm/s^2

  // Determine the acceleration.
  float accel = 0;  // mm/s^2
  if (braking_accel < -config::kAccelDefault) {
    accel = braking_accel;
  } else if (current_setpoint < config::kSpeedRotation) {
    accel = config::kAccelDefault;
  }

  // Calculate the new setpoint.
  // Starting with the current setpoint.
  float left_speed  = current_setpoint;
  float right_speed = current_setpoint;

  // Add the acceleration.
  left_speed  += (accel * config::kControlPeriod);  // mm/s
  right_speed += (accel * config::kControlPeriod);  // mm/s

  // Adjust for the wheel bias.
  left_speed  *= (1.0 - config::kWheelBias);
  right_speed *= (1.0 + config::kWheelBias);

  // Convert to RPMs.
  left_speed  = config::ClampRPM(config::SpeedToRPM(left_speed));
  right_speed = config::ClampRPM(config::SpeedToRPM(right_speed));

  if (s.direction) {
    control::speed::Set(-left_speed, right_speed);
  } else {
    control::speed::Set(left_speed, -right_speed);
  }

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state_ = s;
  }

  return false;
}

void Read(State &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state_;
  }
}

}  // namespace mouse::control::rotational
