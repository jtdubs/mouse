//
// System: utils
// Module: pid
//
// Purpose:
// - Provide PID controllers and variants to the rest of the control system.
//
#pragma once

#include <math.h>

#include "math.hh"

namespace pid {

// PID controller for a process variable.
class PIDController {
 private:
  float min, max;    // bounds for pid output
  float kp, ki, kd;  // pid constants
  float i;           // integral
  float last_pv;     // last process variable

 public:
  PIDController();
  inline float Update(float sp /* setpoint */, float pv /* process variable */);
  void         Reset();
  void         Tune(float kp, float ki, float kd);
  void         SetRange(float min, float max);
};

// PI controller for a process variable.
class PIController {
 private:
  float min, max;  // bounds for pi output
  float kp, ki;    // pi constants
  float i;         // integral

 public:
  PIController();
  inline float Update(float sp /* setpoint */, float pv /* process variable */);
  void         Reset();
  void         Tune(float kp, float ki, float kd);
  void         SetRange(float min, float max);
};

// PD controller for a process variable.
class PDController {
 private:
  float min, max;  // bounds for pd output
  float kp, kd;    // pd constants
  float last_pv;   // last process variable

 public:
  PDController();
  inline float Update(float sp /* setpoint */, float pv /* process variable */);
  void         Reset();
  void         Tune(float kp, float ki, float kd);
  void         SetRange(float min, float max);
};

// P controller for a process variable.
class PController {
 private:
  float min, max;  // bounds for pid output
  float kp;        // pid constants

 public:
  PController();
  inline float Update(float sp /* setpoint */, float pv /* process variable */);
  void         Reset();
  void         Tune(float kp, float ki, float kd);
  void         SetRange(float min, float max);
};

// update determines the manipulated value, given a setpoint and process variable.
inline float PIDController::Update(float sp /* setpoint */, float pv /* process variable */) {
  float p    = sp - pv;       // proportional term is the error
  float newI = i + p;         // integral term is the sum of all errors
  float d    = pv - last_pv;  // derivative term is the change in the process variable

  // NOTE: The derivative term is classicaly the change in the error, but this is
  // sensitive to changes in the setpoint.  The change in the process variable, however,
  // is always continuous.  This is known as "setpoint weighting".

  last_pv = pv;

  // out = kp * p + ki * i + kd * d
  float out = fmaf(kp, p, fmaf(ki, newI, kd * d));

  if (out >= min && out < max) {
    // Only update the integral if the output is within the min/max range.
    // This helps prevent the integral from running away, and is known as
    // "integral anti-windup".
    i = newI;
  }

  return math::clampf(out, min, max);
}

// update determines the manipulated value, given a setpoint and process variable.
inline float PIController::Update(float sp /* setpoint */, float pv /* process variable */) {
  float p    = sp - pv;  // proportional term is the error
  float newI = i + p;    // integral term is the sum of all errors

  // out = kp * p + ki * i
  float out = fmaf(kp, p, ki * newI);

  if (out >= min && out < max) {
    // Only update the integral if the output is within the min/max range.
    // This helps prevent the integral from running away, and is known as
    // "integral anti-windup".
    i = newI;
  }

  return math::clampf(out, min, max);
}

// update determines the manipulated value, given a setpoint and process variable.
inline float PDController::Update(float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;       // proportional term is the error
  float d = pv - last_pv;  // derivative term is the change in the process variable

  // NOTE: The derivative term is classicaly the change in the error, but this is
  // sensitive to changes in the setpoint.  The change in the process variable, however,
  // is always continuous.  This is known as "setpoint weighting".

  last_pv = pv;

  // out = kp * p + kd * d
  float out = fmaf(kp, p, kd * d);

  return math::clampf(out, min, max);
}

// update determines the manipulated value, given a setpoint and process variable.
inline float PController::Update(float sp /* setpoint */, float pv /* process variable */) {
  // out = kp * p
  float out = kp * (sp - pv);

  return math::clampf(out, min, max);
}

}  // namespace pid
