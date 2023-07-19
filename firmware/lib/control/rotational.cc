#include <stdbool.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/math.hh"
#include "position_impl.hh"
#include "rotational_impl.hh"
#include "speed_impl.hh"

namespace rotational {

namespace {
State state;
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
    state = s;
  }
}

bool Tick() {
  float speed_measured_left, speed_measured_right;
  speed::Read(speed_measured_left, speed_measured_right);

  float speed_setpoint_left, speed_setpoint_right;
  speed::ReadSetpoints(speed_setpoint_left, speed_setpoint_right);

  float position_distance, position_theta;
  position::Read(position_distance, position_theta);

  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  float dtheta = s.target_theta - position_theta;
  s.direction  = dtheta > 0;

  // If we are in the right range...
  if (fabsf(dtheta) <= (kCountTheta * 12.0)) {
    // Stop the motors.
    speed::Set(0, 0);
    // We are done when the measured speed < 0.1mm/s
    return (fabsf(RPMToSpeed(speed_measured_left)) < 0.1) &&  //
           (fabsf(RPMToSpeed(speed_measured_right)) < 0.1);
  }

  float current_setpoint = RPMToSpeed(fabsf(speed_setpoint_left));  // mm/s

  // Compute the braking distance.
  float dV            = -current_setpoint;                                             // mm/s
  float dX            = fabsf(s.target_theta - position_theta) * (kWheelBase / 2.0f);  // mm
  float braking_accel = ((2.0f * current_setpoint * dV) + (dV * dV)) / (2.0f * dX);    // mm/s^2

  // Determine the acceleration.
  float accel = 0;  // mm/s^2
  if (braking_accel < -kAccelDefault) {
    accel = braking_accel;
  } else if (current_setpoint < kSpeedRotation) {
    accel = kAccelDefault;
  }

  // Calculate the new setpoint.
  // Starting with the current setpoint.
  float left_speed  = current_setpoint;
  float right_speed = current_setpoint;

  // Add the acceleration.
  left_speed  += (accel * kControlPeriod);  // mm/s
  right_speed += (accel * kControlPeriod);  // mm/s

  // Adjust for the wheel bias.
  left_speed  *= (1.0 - kWheelBias);
  right_speed *= (1.0 + kWheelBias);

  // Convert to RPMs.
  left_speed  = ClampRPM(SpeedToRPM(left_speed));
  right_speed = ClampRPM(SpeedToRPM(right_speed));

  if (s.direction) {
    speed::Set(-left_speed, right_speed);
  } else {
    speed::Set(left_speed, -right_speed);
  }

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  return false;
}

void Read(State &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }
}

}  // namespace rotational
