.PHONY: all clean upload

SOURCES = $(wildcard *.c)
TARGET = cmouse

CFLAGS=-I/usr/lib/avr/include # Include AVR libc headers.
CFLAGS=-I/usr/include/simavr # Include simavr headers.
CFLAGS+=-std=gnu11 # Use C17 standard. 
CFLAGS+=-Og -g2 # Optimize for debugging.
CFLAGS+=-Wall -Werror -Wextra  # Enable all warnings and treat them as errors.
CFLAGS+=-D__AVR__ # Tell AVR libc that we are compiling for AVR.
CFLAGS+=-D__AVR_ATmega328P__ # Tell AVR libc which microcontroller we are using.
CFLAGS+=-DF_CPU=16000000UL # Tell AVR libc what the CPU frequency is.
#CFLAGS+=-ffunction-sections -fdata-sections # Separate functions and data into their own sections to improve LTO.
CFLAGS+=-mmcu=atmega328p  # Target microcontroller.
CFLAGS+=-gdwarf-2 -ggdb3 # Generate debug info in DWARF2 and GDB3 formats.
CFLAGS+=-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 # Define mmcu section.

#LDFLAGS=-fno-fat-lto-objects -flto # Enable link-time optimization.
LDFLAGS+=-Wl,--relax,--gc-sections # Remove unused sections.

all: $(TARGET).elf $(TARGET).hex $(TARGET).s

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).s

upload: $(TARGET).hex
	avrdude -c arduino -P /dev/ttyNano -b 115200 -p m328p -D -U flash:w:$^:i

$(TARGET).elf: $(SOURCES)
	avr-gcc $(CFLAGS) $(LDFLAGS) $^ -o $@

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .text -j .data -j .eeprom -O ihex $< $@

$(TARGET).s: $(TARGET).elf
	avr-objdump -j .text -j .data -j .bss -j .mmcu -d $< > $@
