#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/lib/utils/pid.hh"
#include "firmware/platform/platform.hh"
#include "linear_impl.hh"
#include "position.hh"
#include "sensor_cal.hh"
#include "speed_impl.hh"
#include "walls_impl.hh"

namespace mouse::control::linear {

namespace {
State state_;

#if defined(ALLOW_WALL_PID_TUNING)
float              wall_alpha_;
pid::PIDController wall_error_pid_;
#else
pid::PIController wall_error_pid_;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
float              start_theta_;  // radians
float              angle_alpha_;  // radians
float              angle_error_;  // radians
pid::PIDController angle_error_pid_;
#endif
}  // namespace

void Init() {
  wall_error_pid_.Tune(config::kWallKp, config::kWallKi, config::kWallKd);
  wall_error_pid_.SetRange(-100, 100);
#if defined(ALLOW_WALL_PID_TUNING)
  wall_alpha_ = kWallAlpha;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
  angle_error_pid_.Tune(kAngleKp, kAngleKi, kAngleKd);
  angle_error_pid_.SetRange(-100, 100);
  angle_alpha_ = kAngleAlpha;
#endif
}

void Start(float position /* mm */, bool stop) {
  float position_distance, position_theta;
  position::Read(position_distance, position_theta);

  State s;
  s.target_position = position;                           // mm
  s.target_speed    = stop ? 0.0 : config::kSpeedCruise;  // mm/s
  s.wall_error      = 0;
  s.leds_prev_state = platform::led::IsSet(platform::led::LED::IR);

#if defined(ALLOW_ANGLE_PID_TUNING)
  start_theta_ = position_theta;  // radians
  angle_error_ = 0.0;
#endif

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state_ = s;
  }

  platform::led::Set(platform::led::LED::IR);
}

bool Tick() {
  auto forward = platform::adc::Read(platform::adc::Channel::SensorForward);

  float speed_measured_left, speed_measured_right;
  speed::Read(speed_measured_left, speed_measured_right);

  float speed_setpoint_left, speed_setpoint_right;
  speed::ReadSetpoints(speed_setpoint_left, speed_setpoint_right);

  float position_distance, position_theta;
  position::Read(position_distance, position_theta);

  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state_;
  }

  // Emergency stop if too close to a wall.
  if (forward >= config::kSensorEmergencyStop) {
    platform::led::Set(platform::led::LED::IR, s.leds_prev_state);
    speed::Set(0, 0);
    return true;
  }

  // If we are there, then we are done.
  if (position_distance >= s.target_position) {
    float rpm = config::SpeedToRPM(s.target_speed);
    platform::led::Set(platform::led::LED::IR, s.leds_prev_state);
    speed::Set(rpm, rpm);
    if (s.target_speed == 0.0f) {
      // We are done when the measured speed < 0.1mm/s
      return (config::RPMToSpeed(speed_measured_left) < 0.1) &&  //
             (config::RPMToSpeed(speed_measured_right) < 0.1);
    } else {
      return true;
    }
  }

  float current_speed    = config::RPMToSpeed((speed_measured_left + speed_measured_right) / 2.0f);  // mm/s
  float current_setpoint = config::RPMToSpeed((speed_setpoint_left + speed_setpoint_right) / 2.0f);  // mm/s

  // Compute the braking distance.
  float braking_accel = 0;  // mm/s^2
  if (current_speed > s.target_speed) {
    float dV      = s.target_speed - current_speed;                           // mm/s
    float dX      = s.target_position - position_distance;                    // mm
    braking_accel = ((2.0f * current_speed * dV) + (dV * dV)) / (2.0f * dX);  // mm/s^2
  }

  // Determine the acceleration.
  float accel = 0;  // mm/s^2
  if (braking_accel <= -config::kAccelDefault) {
    accel = braking_accel;
  } else if (current_speed < config::kSpeedCruise) {
    accel = config::kAccelDefault;
  }

  // Calculate the new setpoint.
  // Starting with the current setpoint.
  float left_speed  = current_setpoint;
  float right_speed = current_setpoint;

  // Add the acceleration.
  left_speed  += (accel * config::kControlPeriod);  // mm/s
  right_speed += (accel * config::kControlPeriod);  // mm/s

  // Adjust for the wall (centering) error.
#if defined(ALLOW_WALL_PID_TUNING)
  s.wall_error = (wall_alpha_ * walls::error())  //
               + ((1.0f - wall_alpha_) * s.wall_error);
  float wall_adjustment = wall_error_pid_.Update(0.0, s.wall_error);
#else
  s.wall_error = (config::kWallAlpha * walls::error())  //
               + ((1.0f - config::kWallAlpha) * s.wall_error);
  float wall_adjustment = wall_error_pid_.Update(0.0, s.wall_error);
#endif
  left_speed  -= wall_adjustment;
  right_speed += wall_adjustment;

  // Adjust for the angular error.
#if defined(ALLOW_ANGLE_PID_TUNING)
  angle_error_ = (angle_alpha_ * (s.start_theta_ - position_theta))  //
               + ((1.0f - angle_alpha_) * angle_error_);
  float angle_adjustment  = angle_error_pid_.Update(s.start_theta_, position_theta);
  left_speed             -= angle_adjustment;
  right_speed            += angle_adjustment;
#endif

  // Adjust for the wheel bias.
  left_speed  *= (1.0 - config::kWheelBias);
  right_speed *= (1.0 + config::kWheelBias);

  // Convert to RPMs.
  left_speed  = config::ClampRPM(config::SpeedToRPM(left_speed));
  right_speed = config::ClampRPM(config::SpeedToRPM(right_speed));

  speed::Set(left_speed, right_speed);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state_ = s;
  }

  return false;
}

void TuneWallPID([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                 [[maybe_unused]] float alpha) {
#if defined(ALLOW_WALL_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wall_error_pid_.Tune(kp, ki, kd);
    wall_alpha_ = alpha;
  }
#endif
}

void TuneAnglePID([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                  [[maybe_unused]] float alpha) {
#if defined(ALLOW_ANGLE_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    angle_error_pid_.Tune(kp, ki, kd);
    angle_alpha_ = alpha;
  }
#endif
}

// read reads the current linear state.
void Read(State &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state_;
  }
}

}  // namespace mouse::control::linear
