package mouse

import (
	"context"
	"flag"
)

var (
	usb = flag.Bool("usb", true, "Enable usb interface")
)

type Mouse struct {
	USB *USBInterface
}

func New() *Mouse {
	result := &Mouse{}
	if *usb {
		result.USB = NewUSBInterface()
	}
	return result
}

func (m *Mouse) Run(ctx context.Context) {
	m.USB.Run(ctx)
}
