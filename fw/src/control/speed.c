#include "speed.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/pid.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"

// Motor speeds in RPMs.
float speed_measured_left;
float speed_measured_right;

// Motor speed setpointss in RPMs.
float speed_setpoint_left;
float speed_setpoint_right;

// PI controllers for the motors.
pi_t speed_pi_left;
pi_t speed_pi_right;

void speed_init() {
  speed_pi_left.min = 0;
  speed_pi_left.max = MAX_MOTOR_POWER - MIN_MOTOR_POWER;
  speed_pi_left.kp  = SPEED_KP;
  speed_pi_left.ki  = SPEED_KI;

  speed_pi_right.min = 0;
  speed_pi_right.max = MAX_MOTOR_POWER - MIN_MOTOR_POWER;
  speed_pi_right.kp  = SPEED_KP;
  speed_pi_right.ki  = SPEED_KI;
}

void speed_read() {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_measured_left = (SPEED_LOW_PASS_ALPHA * COUNTS_TO_RPM(encoders_left_delta))  //
                        + ((1.0 - SPEED_LOW_PASS_ALPHA) * speed_measured_left);
    speed_measured_right = (SPEED_LOW_PASS_ALPHA * COUNTS_TO_RPM(encoders_right_delta))  //
                         + ((1.0 - SPEED_LOW_PASS_ALPHA) * speed_measured_right);
  }
}

void speed_update() {
  uint8_t power_left;
  if (fabsf(speed_setpoint_left) < MIN_MOTOR_RPM) {
    pi_reset(&speed_pi_left);
    power_left = 0;
  } else {
    power_left = (uint8_t)pi_update(&speed_pi_left, fabsf(speed_setpoint_left), fabsf(speed_measured_left));
    if (power_left != 0) {
      power_left += MIN_MOTOR_POWER;
    }
  }

  uint8_t power_right;
  if (fabsf(speed_setpoint_right) < MIN_MOTOR_RPM) {
    pi_reset(&speed_pi_right);
    power_right = 0;
  } else {
    power_right = (uint8_t)pi_update(&speed_pi_right, fabsf(speed_setpoint_right), fabsf(speed_measured_right));
    if (power_right != 0) {
      power_right += MIN_MOTOR_POWER;
    }
  }

  bool forward_left  = signbitf(speed_setpoint_left) == 0;
  bool forward_right = signbitf(speed_setpoint_right) == 0;
  motor_set(forward_left ? power_left : -power_left,  //
            forward_right ? power_right : -power_right);
}

void speed_set(float left, float right) {
  speed_setpoint_left  = left;
  speed_setpoint_right = right;
}

void speed_set_pi_coefficients(float kp, float ki) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_pi_left.kp = kp;
    speed_pi_left.ki = ki;

    speed_pi_right.kp = kp;
    speed_pi_right.ki = ki;
  }
}
