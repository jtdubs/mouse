package mouse

type CommandType uint8

const (
	ResetCommandType CommandType = iota
	LEDCommandType
	SpeedPIDCommandType
	PowerPlanCommandType
	SpeedPlanCommandType
	LinearPlanCommandType
	RotationalPlanCommandType
	ExecutePlanCommandType
)

type Command interface {
	isCommand() bool
}

type ResetCommand struct {
	Type CommandType
}

func NewResetCommand() ResetCommand {
	return ResetCommand{
		Type: ResetCommandType,
	}
}

func (ResetCommand) isCommand() bool { return true }

type LEDCommand struct {
	Type                     CommandType
	Builtin, Left, Right, IR bool
}

func NewLEDCommand(builtin, left, right, ir bool) LEDCommand {
	return LEDCommand{
		Type:    LEDCommandType,
		Builtin: builtin,
		Left:    left,
		Right:   right,
		IR:      ir,
	}
}

func (c LEDCommand) isCommand() bool { return true }

type SpeedPIDCommand struct {
	Type          CommandType
	Kp, Ki, Alpha float32
}

func NewSpeedPIDCommand(kp, ki, alpha float32) SpeedPIDCommand {
	return SpeedPIDCommand{
		Type:  SpeedPIDCommandType,
		Kp:    kp,
		Ki:    ki,
		Alpha: alpha,
	}
}

func (SpeedPIDCommand) isCommand() bool { return true }

type PowerPlanCommand struct {
	Type        CommandType
	Left, Right int16
}

func NewPowerPlanCommand(left, right int16) PowerPlanCommand {
	return PowerPlanCommand{
		Type:  PowerPlanCommandType,
		Left:  left,
		Right: right,
	}
}

func (PowerPlanCommand) isCommand() bool { return true }

type SpeedPlanCommand struct {
	Type                  CommandType
	LeftSpeed, RightSpeed float32
}

func NewSpeedPlanCommand(leftSpeed, rightSpeed float32) SpeedPlanCommand {
	return SpeedPlanCommand{
		Type:       SpeedPlanCommandType,
		LeftSpeed:  leftSpeed,
		RightSpeed: rightSpeed,
	}
}

func (SpeedPlanCommand) isCommand() bool { return true }

type LinearPlanCommand struct {
	Type     CommandType
	Distance float32
	Stop     bool
}

func NewLinearPlanCommand(distance float32, stop bool) LinearPlanCommand {
	return LinearPlanCommand{
		Type:     LinearPlanCommandType,
		Distance: distance,
		Stop:     stop,
	}
}

func (LinearPlanCommand) isCommand() bool { return true }

type RotationalPlanCommand struct {
	Type   CommandType
	DTheta float32
}

func NewRotationalPlanCommand(dTheta float32) RotationalPlanCommand {
	return RotationalPlanCommand{
		Type:   RotationalPlanCommandType,
		DTheta: dTheta,
	}
}

func (RotationalPlanCommand) isCommand() bool { return true }

type ExecutePlanCommand struct {
	Type CommandType
}

func NewExecutePlanCommand() LinearPlanCommand {
	return LinearPlanCommand{
		Type: ExecutePlanCommandType,
	}
}

func (ExecutePlanCommand) isCommand() bool { return true }
