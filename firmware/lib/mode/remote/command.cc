#include <stddef.h>
#include <util/atomic.h>

#include "command_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"

namespace remote::command {

namespace {
// The encoded and decoded command, and associated state.
Command *command;
}  // namespace

// on_received is the USART0 callback for when a command is received.
void on_received(uint8_t *buffer, [[maybe_unused]] uint8_t size) {
  assert(assert::COMMAND + 0, command == NULL);

  command = (Command *)buffer;
}

// init initializes the command module.
void init() {
  usart0::set_read_callback(on_received);
  usart0::enable_receiver();
}

// processed indicates the command has been processed.
void processed() {
  command = NULL;
  usart0::enable_receiver();
}

// next gets the next command, if one is available.
bool next(Command &c) {
  bool result = false;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (command != NULL) {
      c      = *command;
      result = true;
    }
  }

  return result;
}

}  // namespace remote::command
