#include "command.h"

#include <stddef.h>
#include <util/atomic.h>

#include "platform/usart0.h"
#include "utils/assert.h"

// The encoded and decoded command, and associated state.
volatile command_t *command;
volatile bool       command_available;

// on_command_received is the USART0 callback for when a command is received.
static void on_command_received(uint8_t *buffer, uint8_t size) {
  assert(ASSERT_COMMAND + 0, command == NULL);
  assert(ASSERT_COMMAND + 1, !command_available);
  (void)size;
  command           = (command_t *)buffer;
  command_available = true;
}

// command_init initializes the command module.
void command_init() {
  usart0_set_read_callback(on_command_received);
  usart0_enable_receiver();
}

// command_processed indicates the command has been processed.
void command_processed() {
  command           = NULL;
  command_available = false;
  usart0_enable_receiver();
}
