.PHONY: all sim fw wave

all: sim

sim: fw
	go run . --firmware ../fw/build/debug/mouse.elf

fw:
	make -C ../fw all

wave: gtkwave_trace.fst
	gtkwave sim.gtkw

gtkwave_trace.fst: gtkwave_trace.vcd
	vcd2fst $< $@
