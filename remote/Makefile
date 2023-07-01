.PHONY: sim board wave

sim:
	go run . --port /tmp/simavr-uart0

board:
	go run . --port /dev/ttyNano

wave: mouse.fst
	gtkwave remote.gtkw

mouse.fst: mouse.vcd
	vcd2fst $< $@
