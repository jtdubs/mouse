#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/lib/utils/pid.hh"
#include "firmware/platform/adc.hh"
#include "firmware/platform/pin.hh"
#include "linear_impl.hh"
#include "position.hh"
#include "sensor_cal.hh"
#include "speed_impl.hh"
#include "walls_impl.hh"

namespace linear {

namespace {
state_t state;

#if defined(ALLOW_WALL_PID_TUNING)
float      wall_alpha;
pid::pid_t wall_error_pid;
#else
pid::pi_t wall_error_pid;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
float      start_theta;  // radians
float      angle_alpha;  // radians
float      angle_error;  // radians
pid::pid_t angle_error_pid;
#endif
}  // namespace

void init() {
  wall_error_pid.kp  = WALL_KP;
  wall_error_pid.ki  = WALL_KI;
  wall_error_pid.min = -100;
  wall_error_pid.max = 100;
#if defined(ALLOW_WALL_PID_TUNING)
  wall_error_pid.kd = WALL_KD;
  wall_alpha        = WALL_ALPHA;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
  angle_error_pid.kp  = ANGLE_KP;
  angle_error_pid.ki  = ANGLE_KI;
  angle_error_pid.kd  = ANGLE_KD;
  angle_error_pid.min = -100;
  angle_error_pid.max = 100;
  angle_alpha         = ANGLE_ALPHA;
#endif
}

void start(float position /* mm */, bool stop) {
  float position_distance, position_theta;
  position::read(position_distance, position_theta);

  state_t s;
  s.target_position = position;                   // mm
  s.target_speed    = stop ? 0.0 : SPEED_CRUISE;  // mm/s
  s.wall_error      = 0;
  s.leds_prev_state = pin::is_set(pin::IR_LEDS);

#if defined(ALLOW_ANGLE_PID_TUNING)
  start_theta = position_theta;  // radians
  angle_error = 0.0;
#endif

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  pin::set(pin::IR_LEDS);
}

bool tick() {
  uint16_t forward = adc::read(adc::SENSOR_FORWARD);

  float speed_measured_left, speed_measured_right;
  speed::read(speed_measured_left, speed_measured_right);

  float speed_setpoint_left, speed_setpoint_right;
  speed::read_setpoints(speed_setpoint_left, speed_setpoint_right);

  float position_distance, position_theta;
  position::read(position_distance, position_theta);

  state_t s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  // Emergency stop if too close to a wall.
  if (forward >= SENSOR_EMERGENCY_STOP) {
    pin::set2(pin::IR_LEDS, s.leds_prev_state);
    speed::set(0, 0);
    return true;
  }

  // If we are there, then we are done.
  if (position_distance >= s.target_position) {
    float rpm = SPEED_TO_RPM(s.target_speed);
    pin::set2(pin::IR_LEDS, s.leds_prev_state);
    speed::set(rpm, rpm);
    return true;
  }

  float current_speed    = RPM_TO_SPEED((speed_measured_left + speed_measured_right) / 2.0f);  // mm/s
  float current_setpoint = RPM_TO_SPEED((speed_setpoint_left + speed_setpoint_right) / 2.0f);  // mm/s

  // Compute the braking distance.
  float braking_accel = 0;  // mm/s^2
  if (current_speed > s.target_speed) {
    float dV      = s.target_speed - current_speed;                           // mm/s
    float dX      = s.target_position - position_distance;                    // mm
    braking_accel = ((2.0f * current_speed * dV) + (dV * dV)) / (2.0f * dX);  // mm/s^2
  }

  // Determine the acceleration.
  float accel = 0;  // mm/s^2
  if (braking_accel <= -ACCEL_DEFAULT) {
    accel = braking_accel;
  } else if (current_speed < SPEED_CRUISE) {
    accel = ACCEL_DEFAULT;
  }

  // Calculate the new setpoint.
  // Starting with the current setpoint.
  float left_speed  = current_setpoint;
  float right_speed = current_setpoint;

  // Add the acceleration.
  left_speed  += (accel * CONTROL_PERIOD);  // mm/s
  right_speed += (accel * CONTROL_PERIOD);  // mm/s

  // Adjust for the wall (centering) error.
#if defined(ALLOW_WALL_PID_TUNING)
  s.wall_error = (wall_alpha * walls::error())  //
               + ((1.0f - wall_alpha) * s.wall_error);
  float wall_adjustment = pid::update(wall_error_pid, 0.0, s.wall_error);
#else
  s.wall_error = (WALL_ALPHA * walls::error())  //
               + ((1.0f - WALL_ALPHA) * s.wall_error);
  float wall_adjustment = pid::update(wall_error_pid, 0.0, s.wall_error);
#endif
  left_speed  -= wall_adjustment;
  right_speed += wall_adjustment;

  // Adjust for the angular error.
#if defined(ALLOW_ANGLE_PID_TUNING)
  angle_error = (angle_alpha * (s.start_theta - position_theta))  //
              + ((1.0f - angle_alpha) * angle_error);
  float angle_adjustment  = pid::update(angle_error_pid, s.start_theta, position_theta);
  left_speed             -= angle_adjustment;
  right_speed            += angle_adjustment;
#endif

  // Adjust for the wheel bias.
  left_speed  *= (1.0 - WHEEL_BIAS);
  right_speed *= (1.0 + WHEEL_BIAS);

  // Convert to RPMs.
  left_speed  = CLAMP_RPM(SPEED_TO_RPM(left_speed));
  right_speed = CLAMP_RPM(SPEED_TO_RPM(right_speed));

  speed::set(left_speed, right_speed);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  return false;
}

void wall_tune([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
               [[maybe_unused]] float alpha) {
#if defined(ALLOW_WALL_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wall_error_pid.kp = kp;
    wall_error_pid.ki = ki;
    wall_error_pid.kd = kd;
    wall_alpha        = alpha;
  }
#endif
}

void angle_tune([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                [[maybe_unused]] float alpha) {
#if defined(ALLOW_ANGLE_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    angle_error_pid.kp = kp;
    angle_error_pid.ki = ki;
    angle_error_pid.kd = kd;
    angle_alpha        = alpha;
  }
#endif
}

// read reads the current linear state.
void read(state_t &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }
}

}  // namespace linear
