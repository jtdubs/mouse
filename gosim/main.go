package main

import (
	"context"
	"errors"
	"flag"

	"github.com/jtdubs/mouse/gosim/pkg/sim"
	"github.com/jtdubs/mouse/gosim/pkg/ui"
	"golang.org/x/sync/errgroup"
)

func main() {
	flag.Parse()

	sim := sim.New()
	ui := ui.New(sim)

	exitError := errors.New("exit")

	grp, ctx := errgroup.WithContext(context.Background())
	grp.Go(func() error { sim.Run(ctx); return exitError })
	grp.Go(func() error { ui.Run(ctx); return exitError })
	grp.Wait()
}
