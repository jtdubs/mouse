.PHONY: all release debug clean upload prep sim simattach size

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
RELEASE_OBJECTS = $(SOURCES:src/%.c=build/release/%.o)
DEBUG_OBJECTS = $(SOURCES:src/%.c=build/debug/%.o)
TARGET = mouse

CFLAGS+=$(shell pkg-config --cflags simavr) # Include simavr headers.
CFLAGS+=-std=c17                       # Use C17 standard. 
CFLAGS+=-Wall -Werror -Wextra          # Enable all warnings and treat them as errors.
CFLAGS+=-D__AVR__                      # Tell AVR libc that we are compiling for AVR.
CFLAGS+=-D__AVR_ATmega328P__           # Tell AVR libc which microcontroller we are using.
CFLAGS+=-DF_CPU=16000000UL             # Tell AVR libc what the CPU frequency is.
CFLAGS+=-mmcu=atmega328p               # Target microcontroller.
CFLAGS+=-Wl,--relax,--gc-sections      # Remove unused sections.
CFLAGS+=-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 # Define mmcu section.
CFLAGS+=-DBASE64_NO_LOOKUPS            # Use slower but smaller base64 implementation.
CFLAGS+=--param=min-pagesize=0         # Needed for AVR target.
CFLAGS+=-O2                            # Optimize for speed.

DBG_CFLAGS=-DDEBUG
DBG_CFLAGS+=-gdwarf-2 -ggdb3           # Generate debug info in DWARF2 and GDB3 formats.

REL_CFLAGS=-DNDEBUG
REL_CFLAGS+=-ffunction-sections        # Separate functions into their own sections to improve LTO.
REL_CFLAGS+=-fdata-sections            # Separate data into their own sections to improve LTO.
REL_CFLAGS+=-fno-fat-lto-objects -flto # Enable link-time optimization.

REL_TARGETS=build/release/$(TARGET).elf build/release/$(TARGET).hex build/release/$(TARGET).s
DBG_TARGETS=build/debug/$(TARGET).elf build/debug/$(TARGET).hex build/debug/$(TARGET).s

all: prep $(REL_TARGETS) $(DBG_TARGETS)

release: prep $(REL_TARGETS)

debug: prep $(DBG_TARGETS)

clean:
	rm -Rf build

upload: prep build/release/$(TARGET).hex
	-killall -HUP -e ui
	avrdude -c arduino -P /dev/ttyNano -b 57600 -p m328p -D -U flash:w:build/release/$(TARGET).hex:i
	-killall -CONT -e ui

prep: build/release build/debug

sim: build/debug/$(TARGET).elf
	../sim/build/release/mousesim $^

simattach: build/debug/$(TARGET).elf
	../sim/build/release/mousesim --gdb $^

size: build/release/$(TARGET).elf
	avr-size $^

build/release:
	mkdir -p build/release

build/debug:
	mkdir -p build/debug

build/release/%.o: src/%.c $(HEADERS)
	avr-gcc $(CFLAGS) $(REL_CFLAGS) -c $< -o $@

build/release/$(TARGET).elf: $(RELEASE_OBJECTS)
	avr-gcc $(CFLAGS) $(REL_CFLAGS) -o $@ $^

build/release/$(TARGET).hex: build/release/$(TARGET).elf
	avr-objcopy -j .text -j .data -j .eeprom -O ihex $^ $@

build/release/$(TARGET).s: build/release/$(TARGET).elf
	avr-objdump -S -j .text -j .data -j .eeprom -j .mmcu -d $^ > $@

build/debug/%.o: src/%.c $(HEADERS)
	avr-gcc $(CFLAGS) $(DBG_CFLAGS) -c $< -o $@

build/debug/$(TARGET).elf: $(DEBUG_OBJECTS)
	avr-gcc $(CFLAGS) $(DBG_CFLAGS) -o $@ $^

build/debug/$(TARGET).hex: build/debug/$(TARGET).elf
	avr-objcopy -j .text -j .data -j .eeprom -O ihex $^ $@

build/debug/$(TARGET).s: build/debug/$(TARGET).elf
	avr-objdump -S -j .text -j .data -j .eeprom -j .mmcu -d $^ > $@
