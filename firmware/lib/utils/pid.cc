#include "pid.hh"

namespace pid {

PIDController::PIDController() {}

void PIDController::Reset() {
  i       = 0;
  last_pv = 0;
}

void PIDController::Tune(float _kp, float _ki, float _kd) {
  kp = _kp;
  ki = _ki;
  kd = _kd;
}

void PIDController::SetRange(float _min, float _max) {
  min = _min;
  max = _max;
}

PIController::PIController() {}

void PIController::Reset() {
  i = 0;
}

void PIController::Tune(float _kp, float _ki, [[maybe_unused]] float kd) {
  kp = _kp;
  ki = _ki;
}

void PIController::SetRange(float _min, float _max) {
  min = _min;
  max = _max;
}

PDController::PDController() {}

void PDController::Reset() {
  last_pv = 0;
}

void PDController::Tune(float _kp, [[maybe_unused]] float ki, float _kd) {
  kp = _kp;
  kd = _kd;
}

void PDController::SetRange(float _min, float _max) {
  min = _min;
  max = _max;
}

PController::PController() {}

void PController::Reset() {}

void PController::Tune(float _kp, [[maybe_unused]] float ki, [[maybe_unused]] float kd) {
  kp = _kp;
}

void PController::SetRange(float _min, float _max) {
  min = _min;
  max = _max;
}

}  // namespace pid
