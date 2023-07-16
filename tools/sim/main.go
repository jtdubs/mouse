package main

import (
	"context"
	"flag"

	"github.com/jtdubs/mouse/tools/sim/pkg/sim"
	"github.com/jtdubs/mouse/tools/sim/pkg/ui"
)

func main() {
	flag.Parse()

	ctx := context.Background()

	sim := sim.New()
	sim.Init()

	ui := ui.New(sim)
	ui.Run(ctx)
}
