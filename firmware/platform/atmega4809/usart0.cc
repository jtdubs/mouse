#include <avr/io.h>
#include <stdint.h>

#include "usart0_impl.hh"

namespace mouse::platform::usart0 {

// Init initializes USART0.
void Init(
  // TODO(justindubs): 4809 impl
 {
#if defined(DEBUG)
  // avrsim doesn't recognize support double speed mode.
  uint16_t baud = (F_CPU / 16 / 115200) - 1;  // 115.2k
#else
  uint16_t baud = (F_CPU / 8 / 115200) - 1;  // 115.2k
#endif

  UBRR0  = baud;
  UCSR0A = _BV(TXC0)                   // Output register is empty.
         | _BV(U2X0);                  // Double speed mode.
  UCSR0B = _BV(RXCIE0)                 // Enable RX Complete Interrupt.
         | _BV(TXEN0);                 // Enable transmitter.
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);  // 8-bit data.
}

}  // namespace mouse::platform::usart0
