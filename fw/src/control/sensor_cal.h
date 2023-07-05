#pragma once

#include <stdbool.h>
#include <stdint.h>

extern uint16_t sensor_threshold_left;    // ADC reading
extern uint16_t sensor_threshold_right;   // ADC reading
extern uint16_t sensor_threshold_center;  // ADC reading

void sensor_cal_init();
void sensor_cal_start();
bool sensor_cal_update();
