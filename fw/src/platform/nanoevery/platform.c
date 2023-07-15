#if defined(BOARD_NANOEVERY)

#include "platform.h"

#include <avr/power.h>
#include <stddef.h>
#include <util/atomic.h>

#include "platform/adc.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"
#include "platform/timer.h"
#include "platform/usart0.h"
#include "utils/assert.h"

#pragma pack(push, 1)
// platform_report_t represents a report of the platform state.
typedef struct {
  uint8_t battery_volts;  // 8-bit ADC value
  struct {
    uint16_t left    : 10;  // 10-bit ADC value
    uint16_t right   : 10;  // 10-bit ADC value
    uint16_t forward : 10;  // 10-bit ADC value
    uint8_t  padding : 2;
  } sensors;
  struct {
    bool    left    : 1;
    bool    right   : 1;
    bool    onboard : 1;
    bool    ir      : 1;
    uint8_t padding : 4;
  } leds;
  struct {
    int32_t left;
    int32_t right;
  } encoders;
  struct {
    int16_t left;
    int16_t right;
  } motors;
} platform_report_t;
#pragma pack(pop)

void platform_init() {
  // Turn off unused hardware to save a few mA.
  power_spi_disable();
  power_twi_disable();

  // Initialize all the platform modules.
  pin_init();
  usart0_init();
  adc_init();
  encoders_init();
  motor_init();
  timer_init();
  rtc_init();
}

uint8_t platform_report(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  assert(ASSERT_PLATFORM + 0, buffer != NULL);
  assert(ASSERT_PLATFORM + 1, len >= sizeof(platform_report_t));

  uint16_t left, right, forward;
  adc_read_sensors(&left, &right, &forward);

  platform_report_t *report = (platform_report_t *)buffer;
  encoders_read(&report->encoders.left, &report->encoders.right);

  motor_read(&report->motors.left, &report->motors.right);

  report->leds.left       = pin_is_set(LED_LEFT);
  report->leds.right      = pin_is_set(LED_RIGHT);
  report->leds.onboard    = pin_is_set(LED_ONBOARD);
  report->leds.ir         = pin_is_set(IR_LEDS);
  report->sensors.left    = left;
  report->sensors.right   = right;
  report->sensors.forward = forward;

  return sizeof(platform_report_t);
}

#endif
