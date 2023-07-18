.PHONY: all clean size upload run monitor bind remote-wave sim-wave

REMOTE_SOURCES=$(shell find ./tools/remote -name "*.go")
SIM_SOURCES=$(shell find ./tools/sim -name "*.go")

all: remote sim
	bazel build //... --platforms=//bazel/platforms:arduino_nano --compilation_mode dbg

clean:
	bazel clean

size:
	avr-size bazel-bin/firmware/mouse

remote: $(REMOTE_SOURCES)
	go build ./tools/remote

sim: $(SIM_SOURCES)
	go build ./tools/sim

upload:
	avrdude -v -c arduino -P /dev/ttyNano -b 115200 -p atmega328p -D -U flash:w:bazel-bin/firmware/mouse.hex:i

run: sim remote
	./sim --firmware bazel-bin/firmware/mouse 2>&1 | tee sim.log &
	./remote --port /tmp/simavr-uart0 2>&1 | tee remote.log &

monitor:
	./remote --port /dev/rfcomm0 2>&1 | tee remote.log &

bind:
	sudo rfcomm bind 0 98:D3:02:96:A7:BF

remote-wave: remote.fst
	gtkwave remote.gtkw

sim-wave: sim.fst
	gtkwave sim.gtkw

%.fst: %.vcd
	vcd2fst $< $@
