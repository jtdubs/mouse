package main

import (
	"context"
	"flag"

	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

func main() {
	flag.Parse()

	ctx, cancelFunc := context.WithCancel(context.Background())
	defer cancelFunc()

	mouse := mouse.New()
	go mouse.Run(ctx)

	ui := newUI()
	ui.run(mouse)
}
