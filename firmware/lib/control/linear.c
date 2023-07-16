#include "linear.h"

#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "config.h"
#include "firmware/lib/utils/assert.h"
#include "firmware/lib/utils/math.h"
#include "firmware/lib/utils/pid.h"
#include "firmware/platform/adc.h"
#include "firmware/platform/pin.h"
#include "position.h"
#include "sensor_cal.h"
#include "speed.h"
#include "walls.h"

static linear_state_t state;

#if defined(ALLOW_WALL_PID_TUNING)
static float linear_wall_alpha;
static pid_t linear_wall_error_pid;
#else
static pi_t linear_wall_error_pid;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
static float linear_start_theta;  // radians
static float linear_angle_alpha;  // radians
static float linear_angle_error;  // radians
static pid_t linear_angle_error_pid;
#endif

void linear_init() {
  linear_wall_error_pid.kp  = WALL_KP;
  linear_wall_error_pid.ki  = WALL_KI;
  linear_wall_error_pid.min = -100;
  linear_wall_error_pid.max = 100;
#if defined(ALLOW_WALL_PID_TUNING)
  linear_wall_error_pid.kd = WALL_KD;
  linear_wall_alpha        = WALL_ALPHA;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
  linear_angle_error_pid.kp  = ANGLE_KP;
  linear_angle_error_pid.ki  = ANGLE_KI;
  linear_angle_error_pid.kd  = ANGLE_KD;
  linear_angle_error_pid.min = -100;
  linear_angle_error_pid.max = 100;
  linear_angle_alpha         = ANGLE_ALPHA;
#endif
}

void linear_start(float position /* mm */, bool stop) {
  float position_distance, position_theta;
  position_read(&position_distance, &position_theta);

  linear_state_t s;
  s.target_position = position;                   // mm
  s.target_speed    = stop ? 0.0 : SPEED_CRUISE;  // mm/s
  s.wall_error      = 0;
  s.leds_prev_state = pin_is_set(IR_LEDS);

#if defined(ALLOW_ANGLE_PID_TUNING)
  linear_start_theta = position_theta;  // radians
  linear_angle_error = 0.0;
#endif

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  pin_set(IR_LEDS);
}

bool linear_tick() {
  uint16_t forward;
  adc_read(ADC_SENSOR_FORWARD, &forward);

  float speed_measured_left, speed_measured_right;
  speed_read(&speed_measured_left, &speed_measured_right);

  float speed_setpoint_left, speed_setpoint_right;
  speed_read_setpoints(&speed_setpoint_left, &speed_setpoint_right);

  float position_distance, position_theta;
  position_read(&position_distance, &position_theta);

  linear_state_t s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  // Emergency stop if too close to a wall.
  if (forward >= SENSOR_EMERGENCY_STOP) {
    pin_set2(IR_LEDS, s.leds_prev_state);
    speed_set(0, 0);
    return true;
  }

  // If we are there, then we are done.
  if (position_distance >= s.target_position) {
    float rpm = SPEED_TO_RPM(s.target_speed);
    pin_set2(IR_LEDS, s.leds_prev_state);
    speed_set(rpm, rpm);
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
  s.wall_error = (linear_wall_alpha * walls_error())  //
               + ((1.0f - linear_wall_alpha) * s.wall_error);
  float wall_adjustment = pid_update(&linear_wall_error_pid, 0.0, s.wall_error);
#else
  s.wall_error = (WALL_ALPHA * walls_error())  //
               + ((1.0f - WALL_ALPHA) * s.wall_error);
  float wall_adjustment = pi_update(&linear_wall_error_pid, 0.0, s.wall_error);
#endif
  left_speed  -= wall_adjustment;
  right_speed += wall_adjustment;

  // Adjust for the angular error.
#if defined(ALLOW_ANGLE_PID_TUNING)
  linear_angle_error = (linear_angle_alpha * (s.start_theta - position_theta))  //
                     + ((1.0f - linear_angle_alpha) * linear_angle_error);
  float angle_adjustment  = pid_update(&linear_angle_error_pid, s.start_theta, position_theta);
  left_speed             -= angle_adjustment;
  right_speed            += angle_adjustment;
#endif

  // Adjust for the wheel bias.
  left_speed  *= (1.0 - WHEEL_BIAS);
  right_speed *= (1.0 + WHEEL_BIAS);

  // Convert to RPMs.
  left_speed  = CLAMP_RPM(SPEED_TO_RPM(left_speed));
  right_speed = CLAMP_RPM(SPEED_TO_RPM(right_speed));

  speed_set(left_speed, right_speed);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  return false;
}

void linear_wall_tune([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                      [[maybe_unused]] float alpha) {
#if defined(ALLOW_WALL_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    linear_wall_error_pid.kp = kp;
    linear_wall_error_pid.ki = ki;
    linear_wall_error_pid.kd = kd;
    linear_wall_alpha        = alpha;
  }
#endif
}

void linear_angle_tune([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                       [[maybe_unused]] float alpha) {
#if defined(ALLOW_ANGLE_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    linear_angle_error_pid.kp = kp;
    linear_angle_error_pid.ki = ki;
    linear_angle_error_pid.kd = kd;
    linear_angle_alpha        = alpha;
  }
#endif
}

// linear_state reads the current linear state.
void linear_state(linear_state_t *s) {
  assert(ASSERT_LINEAR + 0, s != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *s = state;
  }
}
