#include <avr/avr_mcu_section.h>
#include <avr/io.h>

AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);

AVR_MCU_VCD_PORT_PIN('B', 0, "RIGHT_DIR");
AVR_MCU_VCD_PORT_PIN('B', 1, "LEFT_PWM");
AVR_MCU_VCD_PORT_PIN('B', 2, "RIGHT_PWM");
AVR_MCU_VCD_PORT_PIN('B', 5, "LED_BUILTIN");
AVR_MCU_VCD_PORT_PIN('C', 3, "PROBE_0");
AVR_MCU_VCD_PORT_PIN('C', 4, "PROBE_1");
AVR_MCU_VCD_PORT_PIN('C', 5, "PROBE_2");
AVR_MCU_VCD_PORT_PIN('D', 2, "LEFT_CLK_A");
AVR_MCU_VCD_PORT_PIN('D', 3, "RIGHT_CLK_A");
AVR_MCU_VCD_PORT_PIN('D', 4, "LEFT_B");
AVR_MCU_VCD_PORT_PIN('D', 5, "RIGHT_B");
AVR_MCU_VCD_PORT_PIN('D', 6, "USER_IO");
AVR_MCU_VCD_PORT_PIN('D', 7, "LEFT_DIR");

AVR_MCU_VCD_IRQ(TIMER1_COMPA);
AVR_MCU_VCD_IRQ(USART_UDRE);
AVR_MCU_VCD_IRQ(USART_RX);

const struct avr_mmcu_vcd_trace_t _adc_trace[] _MMCU_ = {
    // clang-format off
    { AVR_MCU_VCD_SYMBOL("ADCSRA"), .what = (void*)&ADCSRA },
    { AVR_MCU_VCD_SYMBOL("ADMUX"),  .what = (void*)&ADMUX  },
    { AVR_MCU_VCD_SYMBOL("ADCL"),   .what = (void*)&ADCL   },
    { AVR_MCU_VCD_SYMBOL("ADCH"),   .what = (void*)&ADCH   }
    // clang-format on}
};
