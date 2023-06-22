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
         | (0 << TXCIE0)   // Enable TX Complete Interrupt.
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

// The write buffer and associated state.
static uint8_t *write_buffer;
static uint8_t  write_size;
static uint8_t  write_index;

// READY determines if USART0 is ready for a byte.
#define READY (UCSR0A & (1 << UDRE0))

// usart0_write_ready determines if a write request can be initiated.
bool usart0_write_ready() {
  return write_index == write_size;
}

// usart0_set_write_buffer sets the write buffer for USART0.
void usart0_set_write_buffer(uint8_t *buffer, uint8_t size) {
  assert(ASSERT_USART0 + 0, buffer != NULL);
  assert(ASSERT_USART0 + 1, size > 0);
  assert(ASSERT_USART0 + 2, write_buffer == NULL);

  write_buffer = buffer;
  write_index  = size;
  write_size   = size;
}

// usart0_write begins an asynchronous write to USART0.
void usart0_write() {
  assert(ASSERT_USART0 + 3, write_buffer != NULL);
  assert(ASSERT_USART0 + 4, usart0_write_ready());

  // Enable the Data Register Empty Interrupt to start the write.
  write_index  = 0;
  UCSR0B      |= 1 << UDRIE0;
}

// The read buffer and associated state.
static uint8_t                   *read_buffer;
static uint8_t                    read_size;
static uint8_t                    read_index;
static buffer_received_callback_t read_callback;

// usart0_disable_receiver disables the USART0 receiver.
void usart0_disable_receiver() {
  UCSR0B &= ~(1 << RXEN0);
}

// usart0_enable_receiver enables the USART0 receiver.
void usart0_enable_receiver() {
  assert(ASSERT_USART0 + 5, read_buffer != NULL);
  assert(ASSERT_USART0 + 6, read_callback != NULL);
  assert(ASSERT_USART0 + 7, read_size > 0);

  UCSR0B |= (1 << RXEN0);
}

// usart0_set_read_buffer sets the read buffer for USART0.
void usart0_set_read_buffer(uint8_t *buffer, uint8_t size) {
  assert(ASSERT_USART0 + 8, buffer != NULL);
  assert(ASSERT_USART0 + 9, size > 0);

  read_buffer = buffer;
  read_size   = size;
  read_index  = 0;
}

// usart0_set_read_callback sets the read callback for USART0.
void usart0_set_read_callback(buffer_received_callback_t callback) {
  assert(ASSERT_USART0 + 10, callback != NULL);

  read_callback = callback;
}

// The USART0 Data Register Empty Interrupt.
ISR(USART_UDRE_vect, ISR_BLOCK) {
  if (write_index < write_size) {
    // If there is more to transmit, send the next byte.
    UDR0 = write_buffer[write_index++];
  } else {
    // Otherwise, disable the Data Register Empty Interrupt.
    UCSR0B &= ~(1 << UDRIE0);
  }
}

// The USART0 Receive Complete Interrupt.
ISR(USART_RX_vect, ISR_BLOCK) {
  uint8_t value = UDR0;

  // If we have room in the buffer, store the value.
  if (read_index < read_size) {
    read_buffer[read_index++] = value;
  }

  // If we have a line, call the callback and start the next line.
  if (value == '\n') {
    read_callback(read_index);
    read_index = 0;
  }
}
