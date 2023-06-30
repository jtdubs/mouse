package mouse

type Command interface {
	isCommand() bool
}

type LEDCommand struct {
	Type                     uint8
	Builtin, Left, Right, IR bool
	Padding                  [4]uint8
}

func NewLEDCommand(builtin, left, right, ir bool) LEDCommand {
	return LEDCommand{
		Type:    0,
		Builtin: builtin,
		Left:    left,
		Right:   right,
		IR:      ir,
	}
}

func (c LEDCommand) isCommand() bool { return true }

type PowerCommand struct {
	Type        uint8
	Left, Right int16
	Padding     [4]uint8
}

func NewPowerCommand(left, right int16) PowerCommand {
	return PowerCommand{
		Type:  1,
		Left:  left,
		Right: right,
	}
}

func (c PowerCommand) isCommand() bool { return true }

type SpeedCommand struct {
	Type                  uint8
	LeftSpeed, RightSpeed float32
}

func NewSpeedCommand(leftSpeed, rightSpeed float32) SpeedCommand {
	return SpeedCommand{
		Type:       2,
		LeftSpeed:  leftSpeed,
		RightSpeed: rightSpeed,
	}
}

func (c SpeedCommand) isCommand() bool { return true }
