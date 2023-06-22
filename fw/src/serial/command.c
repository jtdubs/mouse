#include "command.h"

#include "platform/usart0.h"
#include "utils/assert.h"
#include "utils/base64.h"

#define ENCODED_SIZE ((sizeof(command_t) * 4 / 3) + 3)

// The encoded and decoded command, and associated state.
command_t   command;
static bool command_valid;
static char encoded_command[ENCODED_SIZE];
static bool encoded_command_full;

// on_command_received is the USART0 callback for when a command is received.
static void on_command_received(uint8_t size) {
  assert(ASSERT_COMMAND + 0, !encoded_command_full);

  if (size != ENCODED_SIZE) {
    return;
  }

  if (encoded_command[0] != '[' || encoded_command[ENCODED_SIZE - 2] != ']') {
    return;
  }

  if (!command_valid) {
    // If there is room for a new command, decode it.
    command_valid = base64_decode((uint8_t*)&encoded_command[1], (uint8_t*)&command, sizeof(command));
  } else {
    // Otherwise we have to keep this in the encoded_command buffer until the
    // previous command is processed.  And we should disable the receiver so
    // as we have nowhere to store another command.
    encoded_command_full = true;
    usart0_disable_receiver();
  }
}

// command_init initializes the command module.
void command_init() {
  // Ensure the command_t is a multiple of 3 bytes, as that encodes easily into base64 w/o padding.
  _Static_assert((sizeof(command_t) % 3) == 0);

  usart0_set_read_buffer((uint8_t*)encoded_command, ENCODED_SIZE);
  usart0_set_read_callback(on_command_received);
  usart0_enable_receiver();
}

// command_available determines if a command is available to be processed.
bool command_available() {
  return command_valid;
}

// command_processed indicates the command has been processed.
void command_processed() {
  command_valid = false;

  // If there is an encoded command in the buffer, decode it and re-enable the receiver.
  if (encoded_command_full) {
    command_valid        = base64_decode((uint8_t*)&encoded_command[1], (uint8_t*)&command, sizeof(command));
    encoded_command_full = false;
    usart0_enable_receiver();
  }
}
