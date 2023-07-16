#include "command.h"

#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.h"
#include "firmware/platform/usart0.h"

// The encoded and decoded command, and associated state.
static command_t *command;

// on_command_received is the USART0 callback for when a command is received.
static void on_command_received(uint8_t *buffer, [[maybe_unused]] uint8_t size) {
  assert(ASSERT_COMMAND + 0, command == NULL);

  command = (command_t *)buffer;
}

// command_init initializes the command module.
void command_init() {
  usart0_set_read_callback(on_command_received);
  usart0_enable_receiver();
}

// command_processed indicates the command has been processed.
void command_processed() {
  command = NULL;
  usart0_enable_receiver();
}

// command_next gets the next command, if one is available.
bool command_next(command_t *c) {
  assert(ASSERT_COMMAND + 2, c != NULL);

  bool result = false;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (command != NULL) {
      *c     = *command;
      result = true;
    }
  }

  return result;
}
