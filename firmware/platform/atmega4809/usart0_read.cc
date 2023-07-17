#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.h"
#include "usart0_int.h"

constexpr uint8_t MAX_READ_SIZE = 64;

// Read states
typedef enum : uint8_t {
  READ_IDLE,
  READ_LENGTH,
  READ_DATA,
  READ_CHECKSUM,
} read_state_t;

// The read buffer and associated state.
static uint8_t         usart0_read_buffer[MAX_READ_SIZE];
static uint8_t         usart0_read_index;
static uint8_t         usart0_read_length;
static read_callback_t usart0_read_callback;
static read_state_t    usart0_read_state;
static uint8_t         usart0_read_checksum;

// usart0_disable_receiver disables the USART0 receiver.
void usart0_disable_receiver() {
  UCSR0B &= ~_BV(RXEN0);
}

// usart0_enable_receiver enables the USART0 receiver.
void usart0_enable_receiver() {
  assert(ASSERT_USART0_READ + 0, usart0_read_callback != NULL);

  UCSR0B |= _BV(RXEN0);
}

// usart0_set_read_callback sets the read callback for USART0.
void usart0_set_read_callback(read_callback_t callback) {
  assert(ASSERT_USART0_READ + 1, callback != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    usart0_read_callback = callback;
  }
}

// The USART0 Receive Complete Interrupt.
ISR(USART_RX_vect, ISR_BLOCK) {
  uint8_t value = UDR0;

  switch (usart0_read_state) {
    case READ_IDLE:
      // If the start byte is received, transition to the next state.
      if (value == START_BYTE) {
        usart0_read_state    = READ_LENGTH;
        usart0_read_checksum = 0;
        usart0_read_index    = 0;
      }
      break;
    case READ_LENGTH:
      // Validate the length and transition to the next state.
      // Fall back to the idle state if the length is invalid.
      usart0_read_length = value;
      usart0_read_state  = READ_DATA;
      if (usart0_read_length == 0 || usart0_read_length > MAX_READ_SIZE) {
        usart0_read_state = READ_IDLE;
      }
      break;
    case READ_DATA:
      // Receive the next byte and update the checksum.
      // Transition to the next state after all bytes are received.
      usart0_read_buffer[usart0_read_index++]  = value;
      usart0_read_checksum                    += value;
      if (usart0_read_index == usart0_read_length) {
        usart0_read_state = READ_CHECKSUM;
      }
      break;
    case READ_CHECKSUM:
      // Receive the checksum, validate it, and return to the idle state.
      usart0_read_checksum += value;
      if (usart0_read_checksum == 0) {
        // If the command is valid:
        // - Disable the receiver (there's nowhere to store the next command).
        // - Invoke the callback.
        usart0_disable_receiver();
        usart0_read_callback(usart0_read_buffer, usart0_read_length);
      }
      usart0_read_state = READ_IDLE;
  }
}
