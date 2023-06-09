package mouse

import (
	"context"
	"flag"

	"golang.org/x/sync/errgroup"
)

var (
	usb = flag.Bool("usb", true, "Enable usb interface")
)

type Mouse struct {
	USB  *USBInterface
	done bool
}

func New() *Mouse {
	result := &Mouse{}
	if *usb {
		result.USB = NewUSBInterface()
	}
	return result
}

func (m *Mouse) Run(ctx context.Context) {
	grp, ctx := errgroup.WithContext(ctx)
	if m.USB != nil {
		grp.Go(func() error { return m.USB.Run(ctx) })
	}
	grp.Wait()
	m.done = true
}

func (m *Mouse) Done() bool {
	return m.done
}
