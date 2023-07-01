package main

import (
	"context"
	"errors"
	"flag"

	"github.com/jtdubs/mouse/remote/pkg/mouse"
	"github.com/jtdubs/mouse/remote/pkg/ui"
	"golang.org/x/sync/errgroup"
)

func main() {
	flag.Parse()

	mouse := mouse.New()
	ui := ui.New(mouse)

	exitError := errors.New("exit")

	grp, ctx := errgroup.WithContext(context.Background())
	grp.Go(func() error { mouse.Run(ctx); return exitError })
	grp.Go(func() error { ui.Run(ctx); return exitError })
	grp.Wait()
}
