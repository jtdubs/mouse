.PHONY: all release debug clean upload prep

SOURCES = $(wildcard *.c)
TARGET = cmouse

CFLAGS=-I/usr/lib/avr/include          # Include AVR libc headers.
CFLAGS+=-I/usr/include/simavr          # Include simavr headers.
CFLAGS+=-std=gnu11                     # Use C17 standard. 
CFLAGS+=-Wall -Werror -Wextra          # Enable all warnings and treat them as errors.
CFLAGS+=-D__AVR__                      # Tell AVR libc that we are compiling for AVR.
CFLAGS+=-D__AVR_ATmega328P__           # Tell AVR libc which microcontroller we are using.
CFLAGS+=-DF_CPU=16000000UL             # Tell AVR libc what the CPU frequency is.
CFLAGS+=-mmcu=atmega328p               # Target microcontroller.
CFLAGS+=-Wl,--relax,--gc-sections      # Remove unused sections.
CFLAGS+=-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 # Define mmcu section.

DBG_CFLAGS=-DDEBUG
DBG_CFLAGS+=-Og -g2                    # Optimize for debugging.
DBG_CFLAGS+=-gdwarf-2 -ggdb3           # Generate debug info in DWARF2 and GDB3 formats.

REL_CFLAGS=-DNDEBUG
REL_CFLAGS+=-O2                        # Optimize for speed.
REL_CFLAGS+=-ffunction-sections        # Separate functions into their own sections to improve LTO.
REL_CFLAGS+=-fdata-sections            # Separate data into their own sections to improve LTO.
REL_CFLAGS+=-fno-fat-lto-objects -flto # Enable link-time optimization.

all: prep release debug

release: prep build/release/$(TARGET).elf build/release/$(TARGET).hex build/release/$(TARGET).s

build/release/$(TARGET).elf: $(SOURCES)
	avr-gcc $(CFLAGS) $(REL_CFLAGS) $(SOURCES) -o $@

build/release/$(TARGET).hex: build/release/$(TARGET).elf
	avr-objcopy -j .text -j .data -j .eeprom -O ihex $^ $@

build/release/$(TARGET).s: build/release/$(TARGET).elf
	avr-objdump -d $^ > $@

debug: prep build/debug/$(TARGET).elf build/debug/$(TARGET).hex build/debug/$(TARGET).s

build/debug/$(TARGET).elf: $(SOURCES)
	avr-gcc $(CFLAGS) $(REL_CFLAGS) $(SOURCES) -o build/debug/$(TARGET).elf

build/debug/$(TARGET).hex: build/debug/$(TARGET).elf
	avr-objcopy -j .text -j .data -j .eeprom -O ihex $^ $@

build/debug/$(TARGET).s: build/debug/$(TARGET).elf
	avr-objdump -d $^ > $@

clean:
	rm -Rf build

upload: prep build/release/$(TARGET).hex
	avrdude -c arduino -P /dev/ttyNano -b 57600 -p m328p -D -U flash:w:$^:i

prep: build/release build/debug

build/release:
	mkdir -p build/release

build/debug:
	mkdir -p build/debug
