#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "usart0_impl.hh"

namespace mouse::platform::usart0 {

namespace {
// The write buffer and associated state.
uint8_t   *write_buffer_;
uint8_t    write_length_;
uint8_t    write_index_;
WriteState write_state_;
uint8_t    write_checksum_;
}  // namespace

// write_ready determines if a write request can be initiated.
bool WriteReady() {
  return write_state_ == WriteState::Idle;
}

// write begins an asynchronous write to USART0.
void Write(uint8_t *buffer, uint8_t length) {
  assert(assert::Module::Usart0Write, 0, buffer != NULL);
  assert(assert::Module::Usart0Write, 1, length > 0);
  assert(assert::Module::Usart0Write, 2, WriteReady());

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Setup the initial write state.
    write_buffer_   = buffer;
    write_length_   = length;
    write_index_    = 0;
    write_state_    = WriteState::Start;
    write_checksum_ = 0;
  }

  // Initiate the write by enabling the Data Register Empty Interrupt.
  // TODO
}

uint8_t byte;

}  // namespace mouse::platform::usart0
