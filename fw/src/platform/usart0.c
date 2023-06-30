#include "platform/usart0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>

#include "platform/pin.h"
#include "utils/assert.h"

// usart0_init initializes USART0.
void usart0_init() {
#if defined(DEBUG)
  // avrsim doesn't recognize support double speed mode.
  uint16_t baud = (F_CPU / 16 / 115200) - 1;  // 115.2k
#else
  uint16_t baud = (F_CPU / 8 / 115200) - 1;  // 115.2k
#endif

  UBRR0  = baud;
  UCSR0A = (1 << TXC0)     // Output register is empty.
         | (1 << U2X0)     // Double speed mode.
         | (0 << MPCM0);   // Disable multi-processor.
  UCSR0B = (1 << RXCIE0)   // Disable RX Complete Interrupt.
         | (0 << TXCIE0)   // Disable TX Complete Interrupt.
         | (0 << UDRIE0)   // Disable Data Register Empty Interrupt.
         | (0 << RXEN0)    // Disable receiver.
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
