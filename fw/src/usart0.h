#pragma once

#include <stdbool.h>
#include <stdint.h>

// usart0_init initializes USART0.
void usart0_init();

// usart0_read_ready determines if USART0 is ready to be written to.
bool usart0_write_ready();

// usart0_set_write_buffer sets the write buffer for USART0.
void usart0_set_write_buffer(uint8_t *buffer, uint8_t size);

// usart0_write begins an asynchronous write to USART0.
void usart0_write();
