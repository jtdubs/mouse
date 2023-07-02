#include "linear.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/position.h"
#include "control/speed.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_start_speed;      // rpm
float linear_target_speed;     // rpm
bool  linear_braking;

static pi_t  linear_pi_theta;
static float linear_theta_setpoint;
static float linear_theta_alpha;
static float linear_theta_measured;

void linear_init() {
  linear_pi_theta.min = -200;
  linear_pi_theta.max = 200;
  linear_pi_theta.kp  = LINEAR_KP;
  linear_pi_theta.ki  = LINEAR_KI;
  linear_theta_alpha  = LINEAR_LOW_PASS_ALPHA;
}

void linear_start(float distance /* mm */, bool stop) {
  linear_start_distance  = position_distance;                                              // mm
  linear_target_distance = position_distance + distance;                                   // mm
  linear_start_speed     = CLAMP_RPM((speed_measured_left + speed_measured_right) / 2.0);  // RPMs
  linear_target_speed    = stop ? 0.0 : SPEED_TO_RPM(SPEED_CRUISE);                        // RPMs
  linear_braking         = false;
  linear_theta_setpoint  = position_theta;
  pi_reset(&linear_pi_theta);
}

bool linear_update() {
  // If we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    speed_set(linear_target_speed, linear_target_speed);
    return true;
  }

  // Compute the braking distance.
  float current_speed = (speed_setpoint_left + speed_setpoint_right) / 2.0;  // RPMs
  float braking_point = +INFINITY;                                           // mm
  if (current_speed > CLAMP_RPM(linear_target_speed)) {
    float brake_time = TICKS_TO_S((current_speed - CLAMP_RPM(linear_target_speed)) / ACCEL_TO_RPM(ACCEL_DEFAULT));  // s
    float brake_distance  = RPM_TO_SPEED(current_speed) * brake_time;       // mm
    brake_distance       -= 0.5 * ACCEL_DEFAULT * brake_time * brake_time;  // mm
    braking_point         = linear_target_distance - brake_distance;        // mm
  }

  float new_speed = SPEED_TO_RPM(SPEED_CRUISE);

  // If we are in the braking zone, decelerate.
  if (position_distance >= braking_point) {
    new_speed      = current_speed - ACCEL_TO_RPM(ACCEL_DEFAULT);  // RPMs
    linear_braking = true;
  }
  // If we are not in the braking zone, and are under cruise speed, accelerate.
  else if (!linear_braking && current_speed < SPEED_TO_RPM(SPEED_CRUISE)) {
    new_speed = current_speed + ACCEL_TO_RPM(ACCEL_DEFAULT);  // RPMs
  }

  // Adjust steering to maintain angle.
  linear_theta_measured = (linear_theta_alpha * position_theta) + ((1.0 - linear_theta_alpha) * linear_theta_measured);
  float steering_adjustment = pi_update(&linear_pi_theta, linear_theta_setpoint, linear_theta_measured);

  speed_set(new_speed - steering_adjustment, new_speed + steering_adjustment);
  speed_update();
  return false;
}

void linear_set_pi_coefficients(float kp, float ki, float alpha) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    linear_theta_alpha = alpha;
    linear_pi_theta.kp = kp;
    linear_pi_theta.ki = ki;
  }
}
