.PHONY: all release debug clean prep

SOURCES = $(wildcard *.c)
TARGET = mousesim

CFLAGS=-std=c17
CFLAGS+=-Wall -Werror -Wextra
CFLAGS+=-Wno-unused-parameter
CFLAGS+=$(shell pkg-config --cflags simavr)
CFLAGS+=$(shell pkg-config --cflags simavrparts)

DBG_CFLAGS=-DDEBUG
DBG_CFLAGS+=-Og -g2 # Optimize for debugging.
DBG_CFLAGS+=-ggdb3  # Generate debug info in DWARF2 and GDB3 formats.

REL_CFLAGS=-DNDEBUG
REL_CFLAGS+=-O2     # Optimize for speed.

LDFLAGS+=$(shell pkg-config --libs simavr)
LDFLAGS+=$(shell pkg-config --libs simavrparts)

REL_TARGETS=build/release/$(TARGET)
DBG_TARGETS=build/debug/$(TARGET)

all: prep $(REL_TARGETS) $(DBG_TARGETS)

release: prep $(REL_TARGETS)

debug: prep $(DBG_TARGETS)

clean:
	rm -Rf build

prep: build/release build/debug

build/release:
	mkdir -p build/release

build/debug:
	mkdir -p build/debug

build/release/$(TARGET): $(SOURCES)
	gcc $(CFLAGS) $(REL_CFLAGS) $(SOURCES) $(LDFLAGS) -o $@

build/debug/$(TARGET): $(SOURCES)
	gcc $(CFLAGS) $(DBG_CFLAGS) $(SOURCES) $(LDFLAGS) -o $@
