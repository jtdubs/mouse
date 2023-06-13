#include "command.h"

#include <assert.h>

#include "base64.h"
#include "usart0.h"

#define ENCODED_SIZE ((sizeof(command_t) * 4 / 3) + 3)

command_t   command;
static bool command_valid = false;
static char encoded_command[ENCODED_SIZE];
static bool encoded_command_full = false;

void on_command_received(uint8_t size) {
  assert(!encoded_command_full);

  if (size != ENCODED_SIZE) {
    return;
  }

  if (encoded_command[0] != '[' || encoded_command[ENCODED_SIZE - 2] != ']') {
    return;
  }

  if (!command_valid) {
    command_valid = base64_decode((uint8_t*)&encoded_command[1], (uint8_t*)&command, sizeof(command));
  } else {
    encoded_command_full = true;
    usart0_disable_receiver();
  }
}

void command_init() {
  usart0_set_read_buffer((uint8_t*)encoded_command, ENCODED_SIZE);
  usart0_set_read_callback(on_command_received);
  usart0_enable_receiver();
}

bool command_available() {
  return command_valid;
}

void command_processed() {
  if (encoded_command_full) {
    command_valid        = base64_decode((uint8_t*)&encoded_command[1], (uint8_t*)&command, sizeof(command));
    encoded_command_full = false;
    usart0_enable_receiver();
  } else {
    command_valid = false;
  }
}
