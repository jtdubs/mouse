#include "sim_impl.h"

// Define the CPU model, frequency and voltages.
AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);

// Right Motor
AVR_MCU_VCD_PORT_PIN('B', 0, "RIGHT_DIR");
AVR_MCU_VCD_PORT_PIN('D', 3, "RIGHT_CLK_A");
AVR_MCU_VCD_PORT_PIN('D', 5, "RIGHT_B");

// Left Motor
AVR_MCU_VCD_PORT_PIN('D', 7, "LEFT_DIR");
AVR_MCU_VCD_PORT_PIN('D', 2, "LEFT_CLK_A");
AVR_MCU_VCD_PORT_PIN('D', 4, "LEFT_B");

// Probes
AVR_MCU_VCD_PORT_PIN('C', 3, "PROBE_TICK");
AVR_MCU_VCD_PORT_PIN('C', 4, "PROBE_PLAN");
AVR_MCU_VCD_PORT_PIN('C', 5, "PROBE_2");

// LEDs
AVR_MCU_VCD_PORT_PIN('B', 3, "LED_LEFT");
AVR_MCU_VCD_PORT_PIN('D', 6, "LED_RIGHT");
AVR_MCU_VCD_PORT_PIN('B', 5, "LED_ONBOARD");
AVR_MCU_VCD_PORT_PIN('B', 4, "IR_LEDS");

// Interrupts
AVR_MCU_VCD_IRQ_TRACE(INT0_vect_num, 1, "LEFT_ENCODER_CLK");
AVR_MCU_VCD_IRQ_TRACE(INT1_vect_num, 1, "RIGHT_ENCODER_CLK");
AVR_MCU_VCD_IRQ_TRACE(TIMER0_COMPA_vect_num, 1, "TICK");
AVR_MCU_VCD_IRQ_TRACE(TIMER2_OVF_vect_num, 1, "RTC");
AVR_MCU_VCD_IRQ_TRACE(USART_UDRE_vect_num, 1, "SERIAL_TX");
AVR_MCU_VCD_IRQ_TRACE(USART_RX_vect_num, 1, "SERIAL_RX");
AVR_MCU_VCD_IRQ_TRACE(ADC_vect_num, 1, "ADC");

// PWM outputs
const struct avr_mmcu_vcd_trace_t PWMTrace[] _MMCU_ = {
    {.tag  = AVR_MMCU_TAG_VCD_TRACE,
     .len  = sizeof(struct avr_mmcu_vcd_trace_t) - 2,
     .mask = 0,
     .what = (void*)&OCR1A,
     .name = "LEFT_PWM"},
    {.tag  = AVR_MMCU_TAG_VCD_TRACE,
     .len  = sizeof(struct avr_mmcu_vcd_trace_t) - 2,
     .mask = 0,
     .what = (void*)&OCR1B,
     .name = "RIGHT_PWM"},
};

// Console and command registers
AVR_MCU_SIMAVR_COMMAND(&SIM_COMMAND_REG);
AVR_MCU_SIMAVR_CONSOLE(&SIM_CONSOLE_REG);
