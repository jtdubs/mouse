#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/lib/utils/pid.hh"
#include "firmware/platform/platform.hh"
#include "linear_impl.hh"
#include "position.hh"
#include "sensor_cal.hh"
#include "speed_impl.hh"
#include "walls_impl.hh"

namespace linear {

namespace {
State state;

#if defined(ALLOW_WALL_PID_TUNING)
float              wall_alpha;
pid::PIDController wall_error_pid;
#else
pid::PIController wall_error_pid;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
float              start_theta;  // radians
float              angle_alpha;  // radians
float              angle_error;  // radians
pid::PIDController angle_error_pid;
#endif
}  // namespace

void Init() {
  wall_error_pid.Tune(kWallKp, kWallKi, kWallKd);
  wall_error_pid.SetRange(-100, 100);
#if defined(ALLOW_WALL_PID_TUNING)
  wall_alpha = kWallAlpha;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
  angle_error_pid.Tune(kAngleKp, kAngleKi, kAngleKd);
  angle_error_pid.SetRange(-100, 100);
  angle_alpha = kAngleAlpha;
#endif
}

void Start(float position /* mm */, bool stop) {
  float position_distance, position_theta;
  position::Read(position_distance, position_theta);

  State s;
  s.target_position = position;                   // mm
  s.target_speed    = stop ? 0.0 : kSpeedCruise;  // mm/s
  s.wall_error      = 0;
  s.leds_prev_state = pin::IsSet(pin::kIRLEDs);

#if defined(ALLOW_ANGLE_PID_TUNING)
  start_theta = position_theta;  // radians
  angle_error = 0.0;
#endif

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  pin::Set(pin::kIRLEDs);
}

bool Tick() {
  auto forward = adc::Read(adc::Channel::SensorForward);

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

  // Emergency stop if too close to a wall.
  if (forward >= kSensorEmergencyStop) {
    pin::Set(pin::kIRLEDs, s.leds_prev_state);
    speed::Set(0, 0);
    return true;
  }

  // If we are there, then we are done.
  if (position_distance >= s.target_position) {
    float rpm = SpeedToRPM(s.target_speed);
    pin::Set(pin::kIRLEDs, s.leds_prev_state);
    speed::Set(rpm, rpm);
    return true;
  }

  float current_speed    = RPMToSpeed((speed_measured_left + speed_measured_right) / 2.0f);  // mm/s
  float current_setpoint = RPMToSpeed((speed_setpoint_left + speed_setpoint_right) / 2.0f);  // mm/s

  // Compute the braking distance.
  float braking_accel = 0;  // mm/s^2
  if (current_speed > s.target_speed) {
    float dV      = s.target_speed - current_speed;                           // mm/s
    float dX      = s.target_position - position_distance;                    // mm
    braking_accel = ((2.0f * current_speed * dV) + (dV * dV)) / (2.0f * dX);  // mm/s^2
  }

  // Determine the acceleration.
  float accel = 0;  // mm/s^2
  if (braking_accel <= -kAccelDefault) {
    accel = braking_accel;
  } else if (current_speed < kSpeedCruise) {
    accel = kAccelDefault;
  }

  // Calculate the new setpoint.
  // Starting with the current setpoint.
  float left_speed  = current_setpoint;
  float right_speed = current_setpoint;

  // Add the acceleration.
  left_speed  += (accel * kControlPeriod);  // mm/s
  right_speed += (accel * kControlPeriod);  // mm/s

  // Adjust for the wall (centering) error.
#if defined(ALLOW_WALL_PID_TUNING)
  s.wall_error = (wall_alpha * walls::error())  //
               + ((1.0f - wall_alpha) * s.wall_error);
  float wall_adjustment = wall_error_pid.Update(0.0, s.wall_error);
#else
  s.wall_error = (kWallAlpha * walls::error())  //
               + ((1.0f - kWallAlpha) * s.wall_error);
  float wall_adjustment = wall_error_pid.Update(0.0, s.wall_error);
#endif
  left_speed  -= wall_adjustment;
  right_speed += wall_adjustment;

  // Adjust for the angular error.
#if defined(ALLOW_ANGLE_PID_TUNING)
  angle_error = (angle_alpha * (s.start_theta - position_theta))  //
              + ((1.0f - angle_alpha) * angle_error);
  float angle_adjustment  = angle_error_pid.Update(s.start_theta, position_theta);
  left_speed             -= angle_adjustment;
  right_speed            += angle_adjustment;
#endif

  // Adjust for the wheel bias.
  left_speed  *= (1.0 - kWheelBias);
  right_speed *= (1.0 + kWheelBias);

  // Convert to RPMs.
  left_speed  = ClampRPM(SpeedToRPM(left_speed));
  right_speed = ClampRPM(SpeedToRPM(right_speed));

  speed::Set(left_speed, right_speed);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  return false;
}

void TuneWallPID([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                 [[maybe_unused]] float alpha) {
#if defined(ALLOW_WALL_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wall_error_pid.Tune(kp, ki, kd);
    wall_alpha = alpha;
  }
#endif
}

void TuneAnglePID([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                  [[maybe_unused]] float alpha) {
#if defined(ALLOW_ANGLE_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    angle_error_pid.Tune(kp, ki, kd);
    angle_alpha = alpha;
  }
#endif
}

// read reads the current linear state.
void Read(State &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }
}

}  // namespace linear
