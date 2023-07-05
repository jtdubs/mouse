.PHONY: sim board wave bt btbind

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

btbind:
	sudo rfcomm bind 0 98:D3:02:96:A7:BF
