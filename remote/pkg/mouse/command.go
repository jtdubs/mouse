package mouse

type CommandType uint8

const (
	ResetCommandType CommandType = iota
	TunePIDCommandType
	EnqueuePlanCommandType
	ExecutePlanCommandType
)

type PidID uint8

const (
	SpeedPid PidID = iota
	WallPid
	AnglePid
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

type TunePIDCommand struct {
	Type              CommandType
	Id                PidID
	Kp, Ki, Kd, Alpha float32
}

func NewTunePIDCommand(id PidID, kp, ki, kd, alpha float32) TunePIDCommand {
	return TunePIDCommand{
		Type:  TunePIDCommandType,
		Id:    id,
		Kp:    kp,
		Ki:    ki,
		Kd:    kd,
		Alpha: alpha,
	}
}

func (TunePIDCommand) isCommand() bool { return true }

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
