.PHONY: sim fw

sim: fw
	go run . --firmware ../fw/build/debug/mouse.elf

fw:
	make -C ../fw all
