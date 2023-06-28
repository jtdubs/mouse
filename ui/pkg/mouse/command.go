package mouse

type Command interface {
	isCommand() bool
}

type ModeCommand struct {
	Type    uint8
	Mode    uint8
	Padding [4]uint8
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
	Padding                  [1]uint8
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

type MotorCommand struct {
	Type                  uint8
	LeftSpeed, RightSpeed int16
	Padding               [1]uint8
}

func NewMotorCommand(leftSpeed, rightSpeed int16) MotorCommand {
	return MotorCommand{
		Type:       2,
		LeftSpeed:  leftSpeed,
		RightSpeed: rightSpeed,
	}
}

func (c MotorCommand) isCommand() bool { return true }
