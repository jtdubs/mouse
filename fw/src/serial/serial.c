#include "serial.h"

#include "serial/command.h"
#include "serial/report.h"

void serial_init() {
  command_init();
  report_init();
}
