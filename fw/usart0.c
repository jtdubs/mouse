#include "usart0.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "pin.h"

void usart0_init() {
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
         | (0 << TXCIE0)   // Enable TX Complete Interrupt.
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
  pin_toggle(PROBE_0);
  uint8_t d = UDR0;
  (void)d;
}

ISR(USART_UDRE_vect, ISR_BLOCK) {
  pin_toggle(PROBE_1);
  UDR0 = 'A';
}
