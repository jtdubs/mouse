#include <avr/avr_mcu_section.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);
AVR_MCU_VCD_PORT_PIN('B', 4, "PB4");
AVR_MCU_VCD_PORT_PIN('B', 5, "PB5");
AVR_MCU_VCD_PORT_PIN('B', 6, "PB6");
AVR_MCU_VCD_PORT_PIN('B', 7, "PB7");

int init_uart0() {
#ifdef DEBUG
  uint16_t baud = (F_CPU / 8 / 115200) - 1;  // 115.2k
#else
  uint16_t baud = (F_CPU / 8 / 115200) - 1;  // 115.2k
#endif

  UBRR0H = baud >> 8;
  UBRR0L = baud & 0xFF;
  UCSR0A = (1 << TXC0)     // Output register is empty.
         | (1 << U2X0)     // Double speed mode.
         | (0 << MPCM0);   // Disable multi-processor.
  UCSR0B = (0 << RXCIE0)   // Disable RX Complete Interrupt.
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

  return 0;
}

int main() {
  init_uart0();
  sei();

  DDRB |= 0xF0;  // Set PB4-7 as output

  for (;;) {
    PORTB ^= 0x40;

    while (!(UCSR0A & (1 << UDRE0))) {}
    UDR0 = 'A';
  }

  return 0;
}

ISR(USART_RX_vect, ISR_BLOCK) {
  PORTB ^= 0x10;
}

ISR(USART_TX_vect, ISR_BLOCK) {
  PORTB ^= 0x20;
}

ISR(USART_UDRE_vect, ISR_BLOCK) {
  PORTB ^= 0x30;
}
