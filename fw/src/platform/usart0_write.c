#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "platform/usart0_int.h"
#include "utils/assert.h"

// Write states
typedef enum : uint8_t {
  WRITE_IDLE,
  WRITE_START,
  WRITE_LENGTH,
  WRITE_DATA,
  WRITE_CHECKSUM,
} write_state_t;

// The write buffer and associated state.
static uint8_t      *usart0_write_buffer;
static uint8_t       usart0_write_length;
static uint8_t       usart0_write_index;
static write_state_t usart0_write_state;
static uint8_t       usart0_write_checksum;

// usart0_write_ready determines if a write request can be initiated.
bool usart0_write_ready() {
  return usart0_write_state == WRITE_IDLE;
}

// usart0_write begins an asynchronous write to USART0.
void usart0_write(uint8_t *buffer, uint8_t length) {
  assert(ASSERT_USART0_WRITE + 0, buffer != NULL);
  assert(ASSERT_USART0_WRITE + 1, length > 0);
  assert(ASSERT_USART0_WRITE + 2, usart0_write_ready());

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Setup the initial write state.
    usart0_write_buffer   = buffer;
    usart0_write_length   = length;
    usart0_write_index    = 0;
    usart0_write_state    = WRITE_START;
    usart0_write_checksum = 0;
  }

  // Initiate the write by enabling the Data Register Empty Interrupt.
  UCSR0B |= 1 << UDRIE0;
}

// The USART0 Data Register Empty Interrupt.
ISR(USART_UDRE_vect, ISR_BLOCK) {
  switch (usart0_write_state) {
    case WRITE_START:
      // Write the start byte and transition to the next state.
      UDR0               = START_BYTE;
      usart0_write_state = WRITE_LENGTH;
      break;
    case WRITE_LENGTH:
      // Write the length byte and transition to the next state.
      UDR0               = usart0_write_length;
      usart0_write_state = WRITE_DATA;
      break;
    case WRITE_DATA:
      // Write the next byte, update the checksum, and transition after all bytes are written.
      uint8_t byte           = usart0_write_buffer[usart0_write_index++];
      UDR0                   = byte;
      usart0_write_checksum += byte;
      if (usart0_write_index == usart0_write_length) {
        usart0_write_state = WRITE_CHECKSUM;
      }
      break;
    case WRITE_CHECKSUM:
      // Write the checksum, disable the interrupt (nothing left to send), and transition to the next state.
      UDR0                = -usart0_write_checksum;
      usart0_write_state  = WRITE_IDLE;
      UCSR0B             &= ~_BV(UDRIE0);
      break;
    case WRITE_IDLE:
      // Should never happens, as the CHECKSUM state disables the interrupt.
      // But if it does, just turn off the interrupt again...
      UCSR0B &= ~_BV(UDRIE0);
      break;
  }
}
