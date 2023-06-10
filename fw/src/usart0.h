#pragma once

#include <stdbool.h>
#include <stdint.h>

// usart0_init initializes USART0.
void usart0_init();

// usart0_read_ready determines if USART0 is ready to be written to.
bool usart0_write_ready();

// usart0_set_write_buffer sets the write buffer for USART0.
void usart0_set_write_buffer(uint8_t* buffer, uint8_t size);

// usart0_write begins an asynchronous write to USART0.
void usart0_write();

typedef void (*command_received_callback_t)(uint8_t size);

void usart0_disable_receiver();
void usart0_enable_receiver();
void usart0_set_read_buffer(uint8_t* buffer, uint8_t size);
void usart0_set_read_callback(command_received_callback_t callback);
