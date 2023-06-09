.PHONY: all clean prep

TARGET=mousesim
SOURCES=$(wildcard *.c)

CFLAGS=-Wall -Werror -Wextra
CFLAGS=-std=c17

LDFLAGS+=$(shell pkg-config --libs simavr)
LDFLAGS+=$(shell pkg-config --libs simavrparts)

CFLAGS+=$(shell pkg-config --cflags simavr)
CFLAGS+=$(shell pkg-config --cflags simavrparts)

all: build/$(TARGET)

clean:
	rm -Rf build

prep: build

build:
	mkdir -p build

build/$(TARGET): build $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@

