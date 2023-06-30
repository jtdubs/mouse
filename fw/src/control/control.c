#include "control.h"

#include "control/speed.h"
#include "platform/pin.h"
#include "platform/timer.h"
#include "serial/report.h"

void control_init() {
  speed_init();
  timer_set_callback(control_update);
}

void control_update() {
  pin_set(PROBE_TICK);
  speed_update();
  report_send();
  pin_clear(PROBE_TICK);
}
