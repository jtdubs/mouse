#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)
typedef struct {
  uint16_t ir_sensor_left;
  uint8_t  padding;
} report_t;
#pragma pack(pop)

void      report_init();
report_t* report_next();
void      report_ready();
