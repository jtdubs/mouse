#pragma once

#include <stdbool.h>
#include <stdint.h>

void usart0_init();
bool usart0_write_ready();
void usart0_set_write_buffer(uint8_t *buffer, uint8_t size);
void usart0_write();
