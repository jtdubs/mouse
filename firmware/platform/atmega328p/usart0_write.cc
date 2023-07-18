#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "usart0_impl.hh"

namespace usart0 {

namespace {
// The write buffer and associated state.
uint8_t   *write_buffer;
uint8_t    write_length;
uint8_t    write_index;
WriteState write_state;
uint8_t    write_checksum;
}  // namespace

// write_ready determines if a write request can be initiated.
bool write_ready() {
  return write_state == WriteState::Idle;
}

// write begins an asynchronous write to USART0.
void write(uint8_t *buffer, uint8_t length) {
  assert(assert::USART0_WRITE + 0, buffer != NULL);
  assert(assert::USART0_WRITE + 1, length > 0);
  assert(assert::USART0_WRITE + 2, write_ready());

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Setup the initial write state.
    write_buffer   = buffer;
    write_length   = length;
    write_index    = 0;
    write_state    = WriteState::Start;
    write_checksum = 0;
  }

  // Initiate the write by enabling the Data Register Empty Interrupt.
  UCSR0B |= 1 << UDRIE0;
}

uint8_t byte;

// The USART0 Data Register Empty Interrupt.
ISR(USART_UDRE_vect, ISR_BLOCK) {
  switch (write_state) {
    case WriteState::Start:
      // Write the start byte and transition to the next state.
      UDR0        = START_BYTE;
      write_state = WriteState::Length;
      break;
    case WriteState::Length:
      // Write the length byte and transition to the next state.
      UDR0        = write_length;
      write_state = WriteState::Data;
      break;
    case WriteState::Data:
      // Write the next byte, update the checksum, and transition after all bytes are written.
      byte            = write_buffer[write_index++];
      UDR0            = byte;
      write_checksum += byte;
      if (write_index == write_length) {
        write_state = WriteState::Checksum;
      }
      break;
    case WriteState::Checksum:
      // Write the checksum, disable the interrupt (nothing left to send), and transition to the next state.
      UDR0         = -write_checksum;
      write_state  = WriteState::Idle;
      UCSR0B      &= ~_BV(UDRIE0);
      break;
    case WriteState::Idle:
      // Should never happens, as the CHECKSUM state disables the interrupt.
      // But if it does, just turn off the interrupt again...
      UCSR0B &= ~_BV(UDRIE0);
      break;
  }
}

}  // namespace usart0
