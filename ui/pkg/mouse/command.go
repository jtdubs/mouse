package mouse

type Command interface {
	isCommand() bool
}

type ModeCommand struct {
	Type    uint8
	Mode    uint8
	Padding [13]uint8
}

func NewModeCommand(mode uint8) ModeCommand {
	return ModeCommand{
		Type: 0,
		Mode: mode,
	}
}

func (c ModeCommand) isCommand() bool { return true }

type LEDCommand struct {
	Type                     uint8
	Builtin, Left, Right, IR bool
	Padding                  [10]uint8
}

func NewLEDCommand(builtin, left, right, ir bool) LEDCommand {
	return LEDCommand{
		Type:    1,
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
	Padding     [10]uint8
}

func NewPowerCommand(left, right int16) PowerCommand {
	return PowerCommand{
		Type:  2,
		Left:  left,
		Right: right,
	}
}

func (c PowerCommand) isCommand() bool { return true }

type SpeedCommand struct {
	Type                  uint8
	LeftSpeed, RightSpeed float32
	Padding               [6]uint8
}

func NewSpeedCommand(leftSpeed, rightSpeed float32) SpeedCommand {
	return SpeedCommand{
		Type:       3,
		LeftSpeed:  leftSpeed,
		RightSpeed: rightSpeed,
	}
}

func (c SpeedCommand) isCommand() bool { return true }

type PositionCommand struct {
	Type                        uint8
	LeftPosition, RightPosition float32
	Padding                     [6]uint8
}

func NewPositionCommand(leftPosition, rightPosition float32) PositionCommand {
	return PositionCommand{
		Type:          4,
		LeftPosition:  leftPosition,
		RightPosition: rightPosition,
	}
}

func (c PositionCommand) isCommand() bool { return true }

type SpeedPIDCommand struct {
	Type    uint8
	P, I, D float32
	Padding [2]uint8
}

func NewSpeedPIDCommand(p, i, d float32) SpeedPIDCommand {
	return SpeedPIDCommand{
		Type: 5,
		P:    p,
		I:    i,
		D:    d,
	}
}

func (c SpeedPIDCommand) isCommand() bool { return true }

type PositionPIDCommand struct {
	Type    uint8
	P, I, D float32
	Padding [2]uint8
}

func NewPositionPIDCommand(p, i, d float32) PositionPIDCommand {
	return PositionPIDCommand{
		Type: 6,
		P:    p,
		I:    i,
		D:    d,
	}
}

func (c PositionPIDCommand) isCommand() bool { return true }
