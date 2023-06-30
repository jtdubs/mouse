#include "control.h"

#include "control/position.h"
#include "control/speed.h"

void control_init() {
  speed_init();
  position_init();
}
