package main

import (
	"context"
	"flag"

	"github.com/jtdubs/mouse/ui/pkg/mouse"
	"github.com/jtdubs/mouse/ui/pkg/ui"
)

func main() {
	flag.Parse()

	ctx, cancelFunc := context.WithCancel(context.Background())
	defer cancelFunc()

	mouse := mouse.New()
	go mouse.Run(ctx)

	ui := ui.New()
	ui.Run(mouse)
}
