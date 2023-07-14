//
// System: control
// Module: sensor_cal
//
// Purpose:
// - Handles sensor calibration plans.
//
// Design:
// - Enables IR LEDs for 256 ticks, and averages readings over that period.
// - This average is used as the baseline for wall detection.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

// sensor_cal_init initializes the sensor calibration module.
void sensor_cal_init();

// sensor_cal_start starts a sensor calibration plan.
void sensor_cal_start();

// sensor_cal_tick updates the platform to perform the calibration.
bool sensor_cal_tick();

// sensor_cal_read reads the calibrated sensor values.
void sensor_cal_read(uint16_t *left, uint16_t *right, uint16_t *center);
