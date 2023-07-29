#include "command.hh"

#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"

namespace remote::command {

namespace {
// The encoded and decoded command, and associated state.
Command *command;
}  // namespace

// on_received is the USART0 callback for when a command is received.
void on_received(uint8_t *buffer, [[maybe_unused]] uint8_t size) {
  assert(assert::Module::Command, 0, command == NULL);

  command = (Command *)buffer;
}

// Init initializes the command module.
void Init() {
  usart0::SetReadCallback(on_received);
  usart0::EnableReceiver();
}

// processed indicates the command has been processed.
void Processed() {
  command = NULL;
  usart0::EnableReceiver();
}

// next gets the next command, if one is available.
bool Next(Command &c) {
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
