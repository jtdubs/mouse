package mouse

type Report struct {
	BatteryVolts uint8
	Mode         uint8
	Sensors      uint32
	LEDs         uint8
	EncoderLeft  uint16
	EncoderRight uint16
	SpeedLeft    uint8
	SpeedRight   uint8
	Forward      uint8
	Padding      uint8
}

func (r *Report) DecodeSensors() (left, center, right uint16) {
	left = uint16(r.Sensors & 0x3ff)
	center = uint16((r.Sensors >> 10) & 0x3ff)
	right = uint16((r.Sensors >> 20) & 0x3ff)
	return
}

func (r *Report) DecodeLEDs() (onboard, left, right, ir bool) {
	onboard = (r.LEDs & 0x01) == 1
	left = ((r.LEDs >> 1) & 0x01) == 1
	right = ((r.LEDs >> 2) & 0x01) == 1
	ir = ((r.LEDs >> 3) & 0x01) == 1
	return
}

func (r *Report) DecodeForward() (left, right bool) {
	left = (r.Forward & 0x01) == 1
	right = ((r.Forward >> 1) & 0x01) == 1
	return
}
