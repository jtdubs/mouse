package mouse

type CommandType uint8

const (
	ResetCommandType CommandType = iota
	SpeedPIDCommandType
	EnqueuePlanCommandType
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

type EnqueuePlanCommand struct {
	Type CommandType
	Plan EncodedPlan
}

func NewEnqueuePlanCommand(plan DecodedPlan) EnqueuePlanCommand {
	return EnqueuePlanCommand{
		Type: EnqueuePlanCommandType,
		Plan: NewEncodedPlan(plan),
	}
}

func (EnqueuePlanCommand) isCommand() bool { return true }

type ExecutePlanCommand struct {
	Type CommandType
}

func NewExecutePlanCommand() ExecutePlanCommand {
	return ExecutePlanCommand{
		Type: ExecutePlanCommandType,
	}
}

func (ExecutePlanCommand) isCommand() bool { return true }
