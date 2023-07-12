#include "linear.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/position.h"
#include "control/sensor_cal.h"
#include "control/speed.h"
#include "control/walls.h"
#include "platform/adc.h"
#include "platform/pin.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_target_speed;     // mm/s
bool  linear_leds_prev_state;

#if defined(ALLOW_WALL_PID_TUNING)
static float linear_wall_alpha;
static pid_t linear_wall_error_pid;
#else
static p_t linear_wall_error_pid;
#endif

#if defined(ALLOW_ANGLE_PID_TUNING)
static float linear_start_theta;  // radians
static float linear_angle_alpha;  // radians
static float linear_angle_error;  // radians
static pid_t linear_angle_error_pid;
#endif

static float linear_wall_error;

static float calculate_wall_error() {
  if (!wall_left_present && !wall_right_present) {
    return 0;
  } else if (wall_left_present && !wall_right_present) {
    return wall_error_left * 2.0f;
  } else if (!wall_left_present && wall_right_present) {
    return wall_error_right * 2.0f;
  } else {
    return (float)(wall_error_left + wall_error_right);
  }
}

void linear_init() {
  linear_wall_error_pid.kp  = WALL_KP;
  linear_wall_error_pid.min = -100;
  linear_wall_error_pid.max = 100;
#if defined(ALLOW_WALL_PID_TUNING)
  linear_wall_error_pid.ki = WALL_KI;
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

void linear_start(float distance /* mm */, bool stop) {
  linear_start_distance  = position_distance;             // mm
  linear_target_distance = position_distance + distance;  // mm
  linear_target_speed    = stop ? 0.0 : SPEED_CRUISE;     // mm/s
  linear_wall_error      = 0;

#if defined(ALLOW_ANGLE_PID_TUNING)
  linear_start_theta = position_theta;  // radians
  linear_angle_error = 0.0;
#endif

  linear_leds_prev_state = pin_is_set(IR_LEDS);
  pin_set(IR_LEDS);
}

bool linear_tick() {
  uint16_t center = adc_values[ADC_SENSOR_CENTER];

  // Emergency stop if too close to a wall.
  if (center >= SENSOR_EMERGENCY_STOP) {
    pin_set2(IR_LEDS, linear_leds_prev_state);
    speed_set(0, 0);
    return true;
  }

  // If we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    float rpm = SPEED_TO_RPM(linear_target_speed);
    pin_set2(IR_LEDS, linear_leds_prev_state);
    speed_set(rpm, rpm);
    return true;
  }

  float current_speed    = RPM_TO_SPEED((speed_measured_left + speed_measured_right) / 2.0f);  // mm/s
  float current_setpoint = RPM_TO_SPEED((speed_setpoint_left + speed_setpoint_right) / 2.0f);  // mm/s

  // Compute the braking distance.
  float braking_accel = 0;  // mm/s^2
  if (current_speed > linear_target_speed) {
    float dV      = linear_target_speed - current_speed;                      // mm/s
    float dX      = linear_target_distance - position_distance;               // mm
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
  linear_wall_error = (linear_wall_alpha * calculate_wall_error())  //
                    + ((1.0f - linear_wall_alpha) * linear_wall_error);
  float wall_adjustment = pid_update(&linear_wall_error_pid, 0.0, linear_wall_error);
#else
  linear_wall_error = (WALL_ALPHA * calculate_wall_error())  //
                    + ((1.0f - WALL_ALPHA) * linear_wall_error);
  float wall_adjustment = p_update(&linear_wall_error_pid, 0.0, linear_wall_error);
#endif
  left_speed  -= wall_adjustment;
  right_speed += wall_adjustment;

  // Adjust for the angular error.
#if defined(ALLOW_ANGLE_PID_TUNING)
  linear_angle_error = (linear_angle_alpha * (linear_start_theta - position_theta))  //
                     + ((1.0f - linear_angle_alpha) * linear_angle_error);
  float angle_adjustment  = pid_update(&linear_angle_error_pid, linear_start_theta, position_theta);
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
