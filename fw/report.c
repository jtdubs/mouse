#include "report.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "base64.h"
#include "usart0.h"

#define ENCODED_SIZE ((sizeof(report_t) * 4 / 3) + 3)

static report_t report;
static char     encoded_report[ENCODED_SIZE];

void report_init() {
  encoded_report[0]                = '[';
  encoded_report[ENCODED_SIZE - 2] = ']';
  encoded_report[ENCODED_SIZE - 1] = '\n';

  usart0_set_write_buffer((uint8_t*)encoded_report, ENCODED_SIZE);
}

report_t* report_next() {
  if (!usart0_write_ready()) {
    return NULL;
  }
  return (report_t*)&report;
}

void report_ready() {
  base64_encode((uint8_t*)&report, (uint8_t*)&encoded_report[1], sizeof(report));
  usart0_write();
}
