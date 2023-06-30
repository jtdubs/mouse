#include "serial.h"

#include "serial/command.h"
#include "serial/report.h"

// serial_init initializes the serial module.
void serial_init() {
  command_init();
  report_init();
}
