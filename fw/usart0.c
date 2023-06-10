#include "usart0.h"

#include <avr/avr_mcu_section.h>
#include <avr/interrupt.h>
#include <avr/io.h>

AVR_MCU_VCD_PORT_PIN('D', 0, "USART0_RX");
AVR_MCU_VCD_PORT_PIN('D', 1, "USART0_TX");
AVR_MCU_VCD_PORT_PIN('B', 0, "RX_LED");
AVR_MCU_VCD_PORT_PIN('B', 1, "TX_LED");

void init_usart0() {
#if defined(DEBUG)
  // avrsim doesn't recognize support double speed mode.
  uint16_t baud = (F_CPU / 16 / 115200) - 1;  // 115.2k
#else
  uint16_t baud = (F_CPU / 8 / 115200) - 1;  // 115.2k
#endif

  UBRR0H = baud >> 8;
  UBRR0L = baud & 0xFF;
  UCSR0A = (1 << TXC0)     // Output register is empty.
         | (1 << U2X0)     // Double speed mode.
         | (0 << MPCM0);   // Disable multi-processor.
  UCSR0B = (1 << RXCIE0)   // Disable RX Complete Interrupt.
         | (1 << TXCIE0)   // Enable TX Complete Interrupt.
         | (1 << UDRIE0)   // Enable Data Register Empty Interrupt.
         | (1 << RXEN0)    // Enable receiver.
         | (1 << TXEN0)    // Enable transmitter.
         | (0 << UCSZ02);  // 8-bit data.
  UCSR0C = (0 << UMSEL01)  // Asynchronous USART.
         | (0 << UMSEL00)  // Asynchronous USART.
         | (0 << UPM01)    // Parity disabled.
         | (0 << UPM00)    // Parity disabled.
         | (0 << USBS0)    // 1 stop bit.
         | (1 << UCSZ01)   // 8-bit data.
         | (1 << UCSZ00)   // 8-bit data.
         | (0 << UCPOL0);  // Clock polarity (ignored in async mode).
}

ISR(USART_RX_vect, ISR_BLOCK) {
  PORTB     ^= 0x01;
  uint8_t d  = UDR0;
  (void)d;
}

ISR(USART_UDRE_vect, ISR_BLOCK) {
  PORTB ^= 0x02;
  UDR0   = 'A';
}
