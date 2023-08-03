#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "usart0_impl.hh"

namespace mouse::platform::usart0 {

namespace {
// The read buffer and associated state.
[[maybe_unused]] uint8_t         read_buffer_[kMaxReadSize];
[[maybe_unused]] uint8_t         read_index_;
[[maybe_unused]] uint8_t         read_length_;
[[maybe_unused]] read_callback_t read_callback_;
[[maybe_unused]] ReadState       read_state_;
[[maybe_unused]] uint8_t         read_checksum_;
}  // namespace

// DisableReceiver disables the USART0 receiver.
void DisableReceiver() {
  // TODO(justindubs): 4809 impl
}

// EnableReceiver enables the USART0 receiver.
void EnableReceiver() {
  assert(assert::Module::Usart0Read, 0, read_callback_ != NULL);
  // TODO(justindubs): 4809 impl
}

// SetReadCallback sets the read callback for USART0.
void SetReadCallback(read_callback_t callback) {
  assert(assert::Module::Usart0Read, 1, callback != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    read_callback_ = callback;
  }
}

}  // namespace mouse::platform::usart0
