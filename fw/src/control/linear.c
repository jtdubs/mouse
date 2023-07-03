#include "linear.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/position.h"
#include "control/speed.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_target_speed;     // rpm

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
  linear_start_distance  = position_distance;                        // mm
  linear_target_distance = position_distance + distance;             // mm
  linear_target_speed    = stop ? 0.0 : SPEED_TO_RPM(SPEED_CRUISE);  // RPMs
  linear_theta_setpoint  = position_theta;
  pi_reset(&linear_pi_theta);
}

bool linear_update() {
  // If we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    speed_set(linear_target_speed, linear_target_speed);
    return true;
  }

  float current_speed    = (speed_measured_left + speed_measured_right) / 2.0;  // RPMs
  float current_setpoint = (speed_setpoint_left + speed_setpoint_right) / 2.0;  // RPMs

  // Compute the braking distance.
  float braking_rate = 0;  // RPMs
  if (current_speed > linear_target_speed) {
    float deltaV = RPM_TO_SPEED(linear_target_speed - current_speed);  // mm/s
    float deltaX = linear_target_distance - position_distance;         // mm
    braking_rate =
        -ACCEL_TO_RPM(((2.0 * RPM_TO_SPEED(current_speed) * deltaV) + (deltaV * deltaV)) / (2.0 * deltaX));  // RPMs
  }

  float new_speed;

  // If we need to brake, do so.
  if (braking_rate > ACCEL_TO_RPM(ACCEL_DEFAULT)) {
    new_speed = current_setpoint - braking_rate;  // RPMs
  }
  // Otherwise, if we are under cruise speed, accelerate.
  else if (current_setpoint < SPEED_TO_RPM(SPEED_CRUISE)) {
    new_speed = current_setpoint + ACCEL_TO_RPM(ACCEL_DEFAULT);  // RPMs
  }
  // Otherwise, continue at cruise speed.
  else {
    new_speed = SPEED_TO_RPM(SPEED_CRUISE);
  }

  // Adjust steering to maintain angle.
  // This should probably just multiply the final speed by 1+-BIAS rather than doing this PI stuff...
  linear_theta_measured = (linear_theta_alpha * position_theta) + ((1.0 - linear_theta_alpha) * linear_theta_measured);
  float steering_adjustment = pi_update(&linear_pi_theta, linear_theta_setpoint, linear_theta_measured);

  float left_speed  = CLAMP_RPM(new_speed - steering_adjustment);
  float right_speed = CLAMP_RPM(new_speed + steering_adjustment);

  speed_set(left_speed, right_speed);
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
