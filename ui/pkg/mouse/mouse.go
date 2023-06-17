package mouse

import (
	"context"
	"flag"
)

var (
	usb = flag.Bool("usb", true, "Enable usb interface")
)

type Mouse struct {
	Serial *SerialInterface
}

func New() *Mouse {
	result := &Mouse{}
	if *usb {
		result.Serial = NewSerialInterface()
	}
	return result
}

func (m *Mouse) Run(ctx context.Context) {
	m.Serial.Run(ctx)
}
