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
static pi_t  speed_pi_left;
static pi_t  speed_pi_right;
static float speed_alpha;

void speed_init() {
  speed_pi_left.min = -200;
  speed_pi_left.max = 200;
  speed_pi_left.kp  = SPEED_KP;
  speed_pi_left.ki  = SPEED_KI;

  speed_pi_right.min = -200;
  speed_pi_right.max = 200;
  speed_pi_right.kp  = SPEED_KP;
  speed_pi_right.ki  = SPEED_KI;

  speed_alpha = SPEED_LOW_PASS_ALPHA;
}

void speed_read() {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_measured_left = (speed_alpha * COUNTS_TO_RPM(encoders_left_delta))  //
                        + ((1.0 - speed_alpha) * speed_measured_left);
    speed_measured_right = (speed_alpha * COUNTS_TO_RPM(encoders_right_delta))  //
                         + ((1.0 - speed_alpha) * speed_measured_right);
  }
}

void speed_update() {
  float setpoint_left = fabsf(speed_setpoint_left);
  float power_left    = LEFT_RPM_TO_POWER(setpoint_left);
  if (setpoint_left < MIN_MOTOR_RPM) {
    pi_reset(&speed_pi_left);
    power_left = 0;
  } else {
    power_left += pi_update(&speed_pi_left, setpoint_left, fabsf(speed_measured_left));
  }

  float setpoint_right = fabsf(speed_setpoint_right);
  float power_right    = RIGHT_RPM_TO_POWER(setpoint_right);
  if (setpoint_right < MIN_MOTOR_RPM) {
    pi_reset(&speed_pi_right);
    power_right = 0;
  } else {
    power_right += pi_update(&speed_pi_right, setpoint_right, fabsf(speed_measured_right));
  }

  bool    forward_left  = signbitf(speed_setpoint_left) == 0;
  bool    forward_right = signbitf(speed_setpoint_right) == 0;
  int16_t left          = (int16_t)lrintf(forward_left ? power_left : -power_left);
  int16_t right         = (int16_t)lrintf(forward_right ? power_right : -power_right);
  motor_set(left, right);
}

void speed_set(float left, float right) {
  speed_setpoint_left  = left;
  speed_setpoint_right = right;
}

void speed_set_pi_coefficients(float kp, float ki, float alpha) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_alpha = alpha;

    speed_pi_left.kp = kp;
    speed_pi_left.ki = ki;

    speed_pi_right.kp = kp;
    speed_pi_right.ki = ki;
  }
}
