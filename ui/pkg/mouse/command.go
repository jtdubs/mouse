package mouse

type Command interface {
	isCommand() bool
}

type ModeCommand struct {
	Type    uint8
	Mode    uint8
	Padding [7]uint8
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
	Padding                  [4]uint8
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

type PositionCommand struct {
	Type                        uint8
	LeftPosition, RightPosition float32
}

func NewPositionCommand(leftPosition, rightPosition float32) PositionCommand {
	return PositionCommand{
		Type:          3,
		LeftPosition:  leftPosition,
		RightPosition: rightPosition,
	}
}

func (c PositionCommand) isCommand() bool { return true }
