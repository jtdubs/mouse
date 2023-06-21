.PHONY: all clean fw run wave

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
