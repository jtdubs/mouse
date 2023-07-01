package mouse

type Command interface {
	isCommand() bool
}

type LEDCommand struct {
	Type                     uint8
	Builtin, Left, Right, IR bool
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

type PowerPlanCommand struct {
	Type        uint8
	Left, Right int16
}

func NewPowerPlanCommand(left, right int16) PowerPlanCommand {
	return PowerPlanCommand{
		Type:  1,
		Left:  left,
		Right: right,
	}
}

func (c PowerPlanCommand) isCommand() bool { return true }

type SpeedPlanCommand struct {
	Type                  uint8
	LeftSpeed, RightSpeed float32
}

func NewSpeedPlanCommand(leftSpeed, rightSpeed float32) SpeedPlanCommand {
	return SpeedPlanCommand{
		Type:       2,
		LeftSpeed:  leftSpeed,
		RightSpeed: rightSpeed,
	}
}

func (c SpeedPlanCommand) isCommand() bool { return true }

type LinearPlanCommand struct {
	Type     uint8
	Distance float32
	Stop     bool
}

func NewLinearPlanCommand(distance float32, stop bool) LinearPlanCommand {
	return LinearPlanCommand{
		Type:     3,
		Distance: distance,
		Stop:     stop,
	}
}

func (c LinearPlanCommand) isCommand() bool { return true }

type ExecutePlanCommand struct {
	Type uint8
}

func NewExecutePlanCommand() LinearPlanCommand {
	return LinearPlanCommand{
		Type: 4,
	}
}

func (c ExecutePlanCommand) isCommand() bool { return true }
