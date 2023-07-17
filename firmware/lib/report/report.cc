#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "firmware/lib/control/control.h"
#include "firmware/lib/maze/maze.h"
#include "firmware/lib/mode/explore/explore.h"
#include "firmware/lib/utils/assert.h"
#include "firmware/platform/platform.h"
#include "firmware/platform/rtc.h"
#include "firmware/platform/timer.h"
#include "firmware/platform/usart0.h"
#include "report_impl.h"

// report_type_t identifies the type of a given report.
typedef enum : uint8_t {
  REPORT_PLATFORM,
  REPORT_CONTROL,
  REPORT_MAZE,
  REPORT_EXPLORE,
} report_type_t;

// report_header_t represents a mouse report header.
#pragma pack(push, 1)
typedef struct {
  report_type_t type;
  uint32_t      rtc_micros;
} report_header_t;
#pragma pack(pop)

// report_t represents a mouse report.
#pragma pack(push, 1)
typedef struct {
  report_header_t header;
  uint8_t         data[64];  // the format of the data is defined by the report type
} report_t;
#pragma pack(pop)

// The report to be sent.
static report_t report;

// report_init initializes the report module.
void report_init() {
  timer_add_callback(report_send);
}

// report_send sends the report, if usart0 is ready.
void report_send() {
  if (!usart0_write_ready()) {
    return;
  }

  uint8_t len              = 0;
  report.header.rtc_micros = rtc_micros();
  if ((len = explore_report(report.data, sizeof(report.data))) > 0) {
    report.header.type = REPORT_EXPLORE;
  } else if ((len = maze_report(report.data, sizeof(report.data))) > 0) {
    report.header.type = REPORT_MAZE;
  } else if ((len = control_report(report.data, sizeof(report.data))) > 0) {
    report.header.type = REPORT_CONTROL;
  } else if ((len = platform_report(report.data, sizeof(report.data))) > 0) {
    report.header.type = REPORT_PLATFORM;
  }

  if (len == 0) {
    return;
  }

  usart0_write((uint8_t*)&report, sizeof(report_header_t) + len);
}
