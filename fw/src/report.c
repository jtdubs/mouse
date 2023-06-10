#include "report.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "base64.h"
#include "usart0.h"

#define ENCODED_SIZE ((sizeof(report_t) * 4 / 3) + 3)

report_t    report;
static char encoded_report[ENCODED_SIZE];

// report_init initializes the report module.
void report_init() {
  assert(sizeof(report_t) % 3 == 0);

  encoded_report[0]                = '[';
  encoded_report[ENCODED_SIZE - 2] = ']';
  encoded_report[ENCODED_SIZE - 1] = '\n';

  usart0_set_write_buffer((uint8_t*)encoded_report, ENCODED_SIZE);
}

// report_send sends the report.
void report_send() {
  assert(report_available());

  base64_encode((uint8_t*)&report, (uint8_t*)&encoded_report[1], sizeof(report));
  usart0_write();
}
