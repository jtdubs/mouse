.PHONY: all clean size upload run monitor bind remote-wave sim-wave db

REMOTE_SOURCES=$(shell find ./tools/remote -name "*.go")

all: remote
	bazel build //firmware/... --platforms=//bazel/platforms:arduino_nano -c opt
	bazel build //firmware/... --platforms=//bazel/platforms:arduino_nano -c dbg
	bazel build //tools/sim -c opt
	bazel build //tools/remote -c dbg

clean:
	bazel clean

size:
	avr-size bazel-out/k8-opt/bin/firmware/mouse

remote: $(REMOTE_SOURCES)
	go build ./tools/remote

upload:
	avrdude -v -c arduino -P /dev/ttyNano -b 115200 -p atmega328p -D -U flash:w:bazel-out/k8-opt/bin/firmware/mouse.hex:i

run: all
	bazel run --run_under="cd $(PWD) &&" -c opt //tools/sim -- --firmware bazel-out/k8-dbg/bin/firmware/mouse 2>&1 | tee sim.log &
	bazel run --run_under="cd $(PWD) &&" -c opt //tools/remote -- --port /tmp/simavr-uart0 2>&1 | tee remote.log &

monitor:
	./remote --port /dev/rfcomm0 2>&1 | tee remote.log &

bind:
	sudo rfcomm bind 0 98:D3:02:96:A7:BF

remote-wave: remote.fst
	gtkwave remote.gtkw

sim-wave: sim.fst
	gtkwave sim.gtkw

db:
	bazel build //tools:compdb
	bazel build //lib:compdb
	bazel build //firmware:compdb --platforms=//bazel/platforms:arduino_nano
	find -L bazel-bin -name "compile_commands.json" | xargs jq -s 'add' > compile_commands.json

%.fst: %.vcd
	vcd2fst $< $@
