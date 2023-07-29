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

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::pid {

#if not defined(__AVR__)
class PIDController;
class PIController;
class PDController;
class PController;

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PIDController *pid);
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PIController *pid);
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PDController *pid);
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PController *pid);
#endif

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

#if not defined(__AVR__)
  friend std::ostream &operator<<(std::ostream &o, const PIDController *pid);
#endif
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

#if not defined(__AVR__)
  friend std::ostream &operator<<(std::ostream &o, const PIController *pid);
#endif
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

#if not defined(__AVR__)
  friend std::ostream &operator<<(std::ostream &o, const PDController *pid);
#endif
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

#if not defined(__AVR__)
  friend std::ostream &operator<<(std::ostream &o, const PController *pid);
#endif
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

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PIDController *pid) {
  o << "pid::PIDController{" << std::endl;
  o << "  min: " << pid->min << std::endl;
  o << "  max: " << pid->max << std::endl;
  o << "  kp: " << pid->kp << std::endl;
  o << "  ki: " << pid->ki << std::endl;
  o << "  kd: " << pid->kd << std::endl;
  o << "  i: " << pid->i << std::endl;
  o << "  last_pv: " << pid->last_pv << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PIController *pid) {
  o << "pid::PIController{" << std::endl;
  o << "  min: " << pid->min << std::endl;
  o << "  max: " << pid->max << std::endl;
  o << "  kp: " << pid->kp << std::endl;
  o << "  ki: " << pid->ki << std::endl;
  o << "  i: " << pid->i << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PDController *pid) {
  o << "pid::PDController{" << std::endl;
  o << "  min: " << pid->min << std::endl;
  o << "  max: " << pid->max << std::endl;
  o << "  kp: " << pid->kp << std::endl;
  o << "  kd: " << pid->kd << std::endl;
  o << "  last_pv: " << pid->last_pv << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const PController *pid) {
  o << "pid::PController{" << std::endl;
  o << "  min: " << pid->min << std::endl;
  o << "  max: " << pid->max << std::endl;
  o << "  kp: " << pid->kp << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace mouse::pid
