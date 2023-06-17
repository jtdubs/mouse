#include "sim.h"

#include <avr/avr_mcu_section.h>
#include <avr/io.h>

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
AVR_MCU_VCD_PORT_PIN('C', 3, "PROBE_0");
AVR_MCU_VCD_PORT_PIN('C', 4, "PROBE_1");
AVR_MCU_VCD_PORT_PIN('C', 5, "PROBE_2");

// LEDs
AVR_MCU_VCD_PORT_PIN('B', 5, "LED_BUILTIN");
AVR_MCU_VCD_PORT_PIN('D', 6, "USER_IO");

// Interrupts
AVR_MCU_VCD_IRQ_TRACE(INT0_vect_num, 1, "LEFT_ENCODER_CLK");
AVR_MCU_VCD_IRQ_TRACE(INT1_vect_num, 1, "RIGHT_ENCODER_CLK");
AVR_MCU_VCD_IRQ_TRACE(TIMER0_COMPA_vect_num, 1, "TICK");
AVR_MCU_VCD_IRQ_TRACE(USART_UDRE_vect_num, 1, "SERIAL_TX");
AVR_MCU_VCD_IRQ_TRACE(USART_RX_vect_num, 1, "SERIAL_RX");

// PWM outputs
const struct avr_mmcu_vcd_trace_t _pwm_trace[] _MMCU_ = {
    // clang-format off
    { AVR_MCU_VCD_SYMBOL("LEFT_PWM"),  .what = (void*)&OCR1A  },
    { AVR_MCU_VCD_SYMBOL("RIGHT_PWM"), .what = (void*)&OCR1B  }
    // clang-format on}
};

AVR_MCU_SIMAVR_COMMAND(&SIM_COMMAND_REG);
AVR_MCU_SIMAVR_CONSOLE(&SIM_CONSOLE_REG);
