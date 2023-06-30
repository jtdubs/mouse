#pragma once

#include <stdbool.h>
#include <stdint.h>

#define START_BYTE 0x02

// usart0_init initializes USART0.
void usart0_init();

// usart0_read_ready determines if USART0 is ready to be written to.
bool usart0_write_ready();

// usart0_write begins an asynchronous write to USART0.
void usart0_write(uint8_t* buffer, uint8_t length);

typedef void (*buffer_received_callback_t)(uint8_t* buffer, uint8_t length);

// usart0_disable_receiver disables the USART0 receiver.
void usart0_disable_receiver();

// usart0_enable_receiver enables the USART0 receiver.
void usart0_enable_receiver();

// usart0_set_read_callback sets the read callback for USART0.
void usart0_set_read_callback(buffer_received_callback_t callback);
