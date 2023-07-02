.PHONY: sim board wave

sim:
	go run . --port /tmp/simavr-uart0

board:
	go run . --port /dev/ttyNano

bt:
	go run . --port /dev/rfcomm0

wave: mouse.fst
	gtkwave remote.gtkw

mouse.fst: mouse.vcd
	vcd2fst $< $@
