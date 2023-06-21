.PHONY: all clean fw run wave

CGO_CFLAGS = $(shell pkg-config --cflags simavr)
CGO_CFLAGS += $(shell pkg-config --cflags simavrparts)
export CGO_CFLAGS

CGO_LDFLAGS = $(shell pkg-config --libs simavr)
CGO_LDFLAGS += $(shell pkg-config --libs simavrparts)
export CGO_LDFLAGS

all: run

clean:
	go clean .

run: sim
	go run . --firmware ../fw/build/debug/mouse.elf

sim: fw
	go build .

fw:
	make -C ../fw all

wave: gtkwave_trace.fst
	gtkwave sim.gtkw

gtkwave_trace.fst: gtkwave_trace.vcd
	vcd2fst $< $@
