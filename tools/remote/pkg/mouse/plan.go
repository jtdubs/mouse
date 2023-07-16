package mouse

import (
	"bytes"
	"encoding/binary"
)

type PlanType uint8

const (
	PlanTypeIdle PlanType = iota
	PlanTypeLEDs
	PlanTypeIR
	PlanTypeFixedPower
	PlanTypeFixedSpeed
	PlanTypeLinearMotion
	PlanTypeRotationalMotion
	PlanTypeSensorCal
)

type PlanState uint8

const (
	PlanStateScheduled PlanState = iota
	PlanStateUnderway
	PlanStateImplemented
)

var PlanStateNames = map[PlanState]string{
	PlanStateScheduled:   "Scheduled",
	PlanStateUnderway:    "Underway",
	PlanStateImplemented: "Implemented",
}

type EncodedPlan struct {
	Type  PlanType
	State PlanState
	Data  [8]byte
}

func NewEncodedPlan(plan DecodedPlan) EncodedPlan {
	var buf bytes.Buffer
	binary.Write(&buf, binary.LittleEndian, plan)

	encodedPlan := EncodedPlan{
		Type:  plan.planType(),
		State: PlanStateScheduled,
	}
	copy(encodedPlan.Data[:], buf.Bytes())
	return encodedPlan
}

func (p EncodedPlan) Decode() DecodedPlan {
	switch p.Type {
	case PlanTypeIdle:
		return PlanIdle{}
	case PlanTypeLEDs:
		var data PlanLEDs
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	case PlanTypeIR:
		var data PlanIR
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	case PlanTypeFixedPower:
		var data PlanFixedPower
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	case PlanTypeFixedSpeed:
		var data PlanFixedSpeed
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	case PlanTypeLinearMotion:
		var data PlanLinearMotion
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	case PlanTypeRotationalMotion:
		var data PlanRotationalMotion
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	case PlanTypeSensorCal:
		var data PlanSensorCal
		if err := binary.Read(bytes.NewReader(p.Data[:]), binary.LittleEndian, &data); err != nil {
			return nil
		}
		return data
	default:
		return nil
	}
}

type DecodedPlan interface {
	planType() PlanType
}

type PlanIdle struct{}

func (PlanIdle) planType() PlanType { return PlanTypeIdle }

type PlanLEDs struct {
	Left, Right, Builtin bool
}

func (PlanLEDs) planType() PlanType { return PlanTypeLEDs }

type PlanIR struct {
	On bool
}

func (PlanIR) planType() PlanType { return PlanTypeIR }

type PlanFixedPower struct {
	Left  int16
	Right int16
}

func (PlanFixedPower) planType() PlanType { return PlanTypeFixedPower }

type PlanFixedSpeed struct {
	Left  float32
	Right float32
}

func (PlanFixedSpeed) planType() PlanType { return PlanTypeFixedSpeed }

type PlanLinearMotion struct {
	Distance float32
	Stop     bool
}

func (PlanLinearMotion) planType() PlanType { return PlanTypeLinearMotion }

type PlanRotationalMotion struct {
	DTheta float32
}

func (PlanRotationalMotion) planType() PlanType { return PlanTypeRotationalMotion }

type PlanSensorCal struct {
}

func (PlanSensorCal) planType() PlanType { return PlanTypeSensorCal }
