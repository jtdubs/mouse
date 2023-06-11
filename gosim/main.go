package main

import (
	"flag"

	"github.com/jtdubs/mouse/gosim/pkg/sim"
)

func main() {
	flag.Parse()

	s := sim.New()
	s.Run()
}
