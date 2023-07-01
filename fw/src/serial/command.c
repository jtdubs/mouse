#include "command.h"

#include <stddef.h>

#include "platform/usart0.h"
#include "utils/assert.h"

// The encoded and decoded command, and associated state.
command_t *command;

// on_command_received is the USART0 callback for when a command is received.
static void on_command_received(uint8_t *buffer, uint8_t size) {
  assert(ASSERT_COMMAND + 0, command == NULL);
  (void)size;
  command = (command_t *)buffer;
}

// command_init initializes the command module.
void command_init() {
  usart0_set_read_callback(on_command_received);
  usart0_enable_receiver();
}

// command_available determines if a command is available to be processed.
bool command_available() {
  return command != NULL;
}

// command_processed indicates the command has been processed.
void command_processed() {
  command = NULL;
  usart0_enable_receiver();
}
