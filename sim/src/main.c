#include <libgen.h>
#include <pthread.h>
#include <signal.h>
#include <simavr/avr_adc.h>
#include <simavr/avr_ioport.h>
#include <simavr/parts/uart_pty.h>
#include <simavr/sim_avr.h>
#include <simavr/sim_elf.h>
#include <simavr/sim_gdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"

avr_t     *avr = NULL;
uart_pty_t pty;

static void sig_int(int signal) {
  if (avr) {
    avr_terminate(avr);
  }
  exit(0);
}

void on_adc_irq(struct avr_irq_t *irq, uint32_t value, void *param) {
  avr_raise_irq(avr_io_getirq(avr, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_ADC0), 1000);
}

void on_led_write(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
  printf("LED Write: %d\n", (v >> 5) & 1);
}

int main(int argc, char *argv[]) {
  arguments_t *args = parse_args(argc, argv);

  printf("Firmware: %s\n", args->firmware_path);
  printf("GDB: %s\n", args->gdb_enabled ? "YES" : "NO");

  int rc;

  elf_firmware_t f;
  if ((rc = elf_read_firmware(args->firmware_path, &f)) != 0) {
    printf("elf_read_firmware failed: %d\n", rc);
    return 1;
  }

  avr = avr_make_mcu_by_name(f.mmcu);
  if (avr == NULL) {
    printf("avr_make_mcu_by_name failed\n");
    return 1;
  }

  if (avr_init(avr) != 0) {
    printf("avr_init failed\n");
    return 1;
  }

  avr_load_firmware(avr, &f);

  avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_OUT_TRIGGER), on_adc_irq, NULL);
  avr_register_io_write(avr, 0x25, on_led_write, NULL);

  if (args->gdb_enabled) {
    avr->state    = cpu_Stopped;
    avr->gdb_port = 1234;
    if (avr_gdb_init(avr) != 0) {
      printf("avr_gdb_init failed\n");
      return 1;
    }
  }

  uart_pty_init(avr, &pty);
  uart_pty_connect(&pty, '0');

  signal(SIGINT, sig_int);
  signal(SIGTERM, sig_int);

  int state = cpu_Running;
  while ((state != cpu_Done) && (state != cpu_Crashed)) {
    state = avr_run(avr);
  }

  sig_int(0);

  return 0;
}
