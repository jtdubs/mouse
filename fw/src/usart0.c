#include "usart0.h"

#include <assert.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "pin.h"

// usart0_init initializes USART0.
void usart0_init() {
#if defined(DEBUG)
  // avrsim doesn't recognize support double speed mode.
  uint16_t baud = (F_CPU / 16 / 115200) - 1;  // 115.2k
#else
  uint16_t baud = (F_CPU / 8 / 115200) - 1;  // 115.2k
#endif

  UBRR0H = baud >> 8;
  UBRR0L = baud & 0xFF;
  UCSR0A = (1 << TXC0)     // Output register is empty.
         | (1 << U2X0)     // Double speed mode.
         | (0 << MPCM0);   // Disable multi-processor.
  UCSR0B = (1 << RXCIE0)   // Disable RX Complete Interrupt.
         | (0 << TXCIE0)   // Enable TX Complete Interrupt.
         | (0 << UDRIE0)   // Disable Data Register Empty Interrupt.
         | (0 << RXEN0)    // Disable receiver.
         | (1 << TXEN0)    // Enable transmitter.
         | (0 << UCSZ02);  // 8-bit data.
  UCSR0C = (0 << UMSEL01)  // Asynchronous USART.
         | (0 << UMSEL00)  // Asynchronous USART.
         | (0 << UPM01)    // Parity disabled.
         | (0 << UPM00)    // Parity disabled.
         | (0 << USBS0)    // 1 stop bit.
         | (1 << UCSZ01)   // 8-bit data.
         | (1 << UCSZ00)   // 8-bit data.
         | (0 << UCPOL0);  // Clock polarity (ignored in async mode).
}

static uint8_t *write_buffer = NULL;
static uint8_t  write_size   = 0;
static uint8_t  write_index  = 0;

// READY determines if USART0 is ready for a byte.
#define READY (UCSR0A & (1 << UDRE0))

// usart0_write_ready determines if USART0 is ready for a write request.
bool usart0_write_ready() {
  return write_index == write_size;
}

// usart0_set_write_buffer sets the write buffer for USART0.
void usart0_set_write_buffer(uint8_t *buffer, uint8_t size) {
  assert(buffer != NULL);
  assert(size > 0);
  assert(write_buffer == NULL);

  write_buffer = buffer;
  write_index  = size;
  write_size   = size;
}

// usart0_write begins an asynchronous write to USART0.
void usart0_write() {
  assert(write_buffer != NULL);
  assert(usart0_write_ready());

  write_index  = 0;
  UCSR0B      |= 1 << UDRIE0;
}

static uint8_t                    *read_buffer   = NULL;
static uint8_t                     read_size     = 0;
static uint8_t                     read_index    = 0;
static command_received_callback_t read_callback = NULL;

// usart0_disable_receiver disables the USART0 receiver.
void usart0_disable_receiver() {
  UCSR0B &= ~(1 << RXEN0);
}

// usart0_enable_receiver enables the USART0 receiver.
void usart0_enable_receiver() {
  assert(read_buffer != NULL);
  assert(read_callback != NULL);
  assert(read_size > 0);

  UCSR0B |= (1 << RXEN0);
}

// usart0_set_read_buffer sets the read buffer for USART0.
void usart0_set_read_buffer(uint8_t *buffer, uint8_t size) {
  assert(buffer != NULL);
  assert(size > 0);

  read_buffer = buffer;
  read_size   = size;
  read_index  = 0;
}

// usart0_set_read_callback sets the read callback for USART0.
void usart0_set_read_callback(command_received_callback_t callback) {
  assert(callback != NULL);

  read_callback = callback;
}

ISR(USART_UDRE_vect, ISR_BLOCK) {
  if (write_index < write_size) {
    UDR0 = write_buffer[write_index++];
    pin_toggle(PROBE_1);
  } else {
    UCSR0B &= ~(1 << UDRIE0);
    pin_clear(PROBE_1);
  }
}

ISR(USART_RX_vect, ISR_BLOCK) {
  uint8_t value = UDR0;

  if (read_index < read_size) {
    read_buffer[read_index++] = value;
    if (value == '\n') {
      if (read_index == read_size) {
        read_callback(read_size);
      }
    }
  }

  if (value == '\n') {
    read_index = 0;
  }
}
