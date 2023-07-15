.PHONY: all release debug clean upload sim simattach size

BOARD=NANO

AVR_BIN=/opt/avr-toolchain/bin

GCC = $(AVR_BIN)/avr-gcc
OBJCOPY = $(AVR_BIN)/avr-objcopy
OBJDUMP = $(AVR_BIN)/avr-objdump

SOURCES = $(shell find src -name "*.c")
HEADERS = $(shell find src -name "*.h")
RELEASE_OBJECTS = $(SOURCES:src/%.c=build/release/%.o)
DEBUG_OBJECTS = $(SOURCES:src/%.c=build/debug/%.o)
TARGET = mouse

CFLAGS=$(shell pkg-config --cflags simavr) # Include simavr headers.
CFLAGS+=-std=c2x                       # Use C17 standard. 
CFLAGS+=-Wall -Werror -Wextra          # Enable all warnings and treat them as errors.
CFLAGS+=-D__AVR__                      # Tell AVR libc that we are compiling for AVR.
CFLAGS+=-DF_CPU=16000000UL             # Tell AVR libc what the CPU frequency is.
CFLAGS+=-DBOARD_$(BOARD)               # Define the board support package to use.
CFLAGS+=-Wl,--relax,--gc-sections      # Remove unused sections.
CFLAGS+=-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 # Define mmcu section.
CFLAGS+=--param=min-pagesize=0         # Needed for AVR target.
CFLAGS+=-O2                            # Optimize for speed.
CFLAGS+=-lm                            # Use AVR math library.
CFLAGS+=-Isrc/                         # Add src/ to include path.

ifeq ($(BOARD),NANO)
CFLAGS+=-D__AVR_ATmega328P__           # Tell AVR libc which microcontroller we are using.
CFLAGS+=-mmcu=atmega328p               # Target microcontroller.
endif
ifeq ($(BOARD),NANOEVERY)
CFLAGS+=-D__AVR_ATmega4809__           # Tell AVR libc which microcontroller we are using.
CFLAGS+=-mmcu=avrxmega3               # Target microcontroller.
endif

DBG_CFLAGS=-DDEBUG
DBG_CFLAGS+=-gdwarf-2 -ggdb3           # Generate debug info in DWARF2 and GDB3 formats.
DBG_CFLAGS+=-Wl,-Map,build/debug/$(TARGET).map  	# Generate map files.

REL_CFLAGS=-DNDEBUG
REL_CFLAGS+=-ffunction-sections        # Separate functions into their own sections to improve LTO.
REL_CFLAGS+=-fdata-sections            # Separate data into their own sections to improve LTO.
REL_CFLAGS+=-fno-fat-lto-objects -flto # Enable link-time optimization.
REL_CFLAGS+=-Wl,-Map,build/release/$(TARGET).map  	# Generate map files.

REL_TARGETS=build/release/$(TARGET).elf build/release/$(TARGET).hex build/release/$(TARGET).s
DBG_TARGETS=build/debug/$(TARGET).elf build/debug/$(TARGET).hex build/debug/$(TARGET).s

all: $(REL_TARGETS) $(DBG_TARGETS)

release: $(REL_TARGETS)

debug: $(DBG_TARGETS)

clean:
	rm -Rf build

upload: build/release/$(TARGET).hex
	-killall -HUP -e remote
	avrdude -v -c arduino -P /dev/ttyNano -b 115200 -p atmega328p -D -U flash:w:build/release/$(TARGET).hex:i
	-killall -CONT -e remote

sim: build/debug/$(TARGET).elf
	../sim/build/release/mousesim $^

simattach: build/debug/$(TARGET).elf
	../sim/build/release/mousesim --gdb $^

size: build/release/$(TARGET).elf
	avr-size $^

build/release/%.o: src/%.c $(HEADERS)
	mkdir -p $(dir $@)
	$(GCC) $(CFLAGS) $(REL_CFLAGS) -c $< -o $@

build/release/$(TARGET).elf: $(RELEASE_OBJECTS)
	mkdir -p $(dir $@)
	$(GCC) $(CFLAGS) $(REL_CFLAGS) -o $@ $^

build/release/$(TARGET).hex: build/release/$(TARGET).elf
	mkdir -p $(dir $@)
	$(OBJCOPY) -j .text -j .data -j .eeprom -O ihex $^ $@

build/release/$(TARGET).s: build/release/$(TARGET).elf
	mkdir -p $(dir $@)
	$(OBJDUMP) -S -j .text -j .data -j .eeprom -j .mmcu -d $^ > $@

build/debug/%.o: src/%.c $(HEADERS)
	mkdir -p $(dir $@)
	$(GCC) $(CFLAGS) $(DBG_CFLAGS) -c $< -o $@

build/debug/$(TARGET).elf: $(DEBUG_OBJECTS)
	mkdir -p $(dir $@)
	$(GCC) $(CFLAGS) $(DBG_CFLAGS) -o $@ $^

build/debug/$(TARGET).hex: build/debug/$(TARGET).elf
	mkdir -p $(dir $@)
	$(OBJCOPY) -j .text -j .data -j .eeprom -O ihex $^ $@

build/debug/$(TARGET).s: build/debug/$(TARGET).elf
	mkdir -p $(dir $@)
	$(OBJDUMP) -S -j .text -j .data -j .eeprom -j .mmcu -d $^ > $@
