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
#if defined(ALLOW_SPEED_PID_TUNING)
static pid_t speed_pid_left;
static pid_t speed_pid_right;
static float speed_alpha;
#else
static pi_t speed_pid_left;
static pi_t speed_pid_right;
#endif

void speed_init() {
  speed_pid_left.min = -200;
  speed_pid_left.max = 200;
  speed_pid_left.kp  = SPEED_KP;
  speed_pid_left.ki  = SPEED_KI;

  speed_pid_right.min = -200;
  speed_pid_right.max = 200;
  speed_pid_right.kp  = SPEED_KP;
  speed_pid_right.ki  = SPEED_KI;

#if defined(ALLOW_SPEED_PID_TUNING)
  speed_alpha        = SPEED_ALPHA;
  speed_pid_left.ki  = SPEED_KD;
  speed_pid_right.ki = SPEED_KD;
#endif
}

void speed_read() {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
#if defined(ALLOW_SPEED_PID_TUNING)
    speed_measured_left = (speed_alpha * COUNTS_TO_RPM(encoders_left_delta))  //
                        + ((1.0f - speed_alpha) * speed_measured_left);
    speed_measured_right = (speed_alpha * COUNTS_TO_RPM(encoders_right_delta))  //
                         + ((1.0f - speed_alpha) * speed_measured_right);
#else
    speed_measured_left = (SPEED_ALPHA * COUNTS_TO_RPM(encoders_left_delta))  //
                        + ((1.0f - SPEED_ALPHA) * speed_measured_left);
    speed_measured_right = (SPEED_ALPHA * COUNTS_TO_RPM(encoders_right_delta))  //
                         + ((1.0f - SPEED_ALPHA) * speed_measured_right);
#endif
  }
}

void speed_update() {
  float setpoint_left = fabsf(speed_setpoint_left);
  float power_left    = LEFT_RPM_TO_POWER(setpoint_left);
  if (setpoint_left < MIN_MOTOR_RPM) {
#if defined(ALLOW_SPEED_PID_TUNING)
    pid_reset(&speed_pid_left);
#else
    pi_reset(&speed_pid_left);
#endif
    power_left = 0;
  } else {
#if defined(ALLOW_SPEED_PID_TUNING)
    power_left += pid_update(&speed_pid_left, setpoint_left, fabsf(speed_measured_left));
#else
    power_left += pi_update(&speed_pid_left, setpoint_left, fabsf(speed_measured_left));
#endif
  }

  float setpoint_right = fabsf(speed_setpoint_right);
  float power_right    = RIGHT_RPM_TO_POWER(setpoint_right);
  if (setpoint_right < MIN_MOTOR_RPM) {
#if defined(ALLOW_SPEED_PID_TUNING)
    pid_reset(&speed_pid_right);
#else
    pi_reset(&speed_pid_right);
#endif
    power_right = 0;
  } else {
#if defined(ALLOW_SPEED_PID_TUNING)
    power_right += pid_update(&speed_pid_right, setpoint_right, fabsf(speed_measured_right));
#else
    power_right += pi_update(&speed_pid_right, setpoint_right, fabsf(speed_measured_right));
#endif
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

void speed_tune([[maybe_unused]] float kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd,
                [[maybe_unused]] float alpha) {
#if defined(ALLOW_SPEED_PID_TUNING)
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_alpha = alpha;

    speed_pid_left.kp = kp;
    speed_pid_left.ki = ki;

    speed_pid_right.kp = kp;
    speed_pid_right.ki = ki;
  }
#endif
}
