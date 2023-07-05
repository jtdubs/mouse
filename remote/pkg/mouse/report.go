package mouse

import (
	"fmt"
	"unsafe"

	"github.com/elamre/vcd"
)

type Report struct {
	// BatteryVolts       uint8
	// Sensors            uint32
	LEDs               uint8
	EncoderLeft        int32
	EncoderRight       int32
	MotorPowerLeft     int16
	MotorPowerRight    int16
	SpeedMeasuredLeft  float32
	SpeedMeasuredRight float32
	SpeedSetpointLeft  float32
	SpeedSetpointRight float32
	PositionDistance   float32
	PositionTheta      float32
	Plan               EncodedPlan
	RTCMicros          uint32
}

func (r *Report) DecodeSensors() (left, center, right uint16) {
	return 0, 0, 0
	// left = uint16(r.Sensors & 0x3ff)
	// center = uint16((r.Sensors >> 10) & 0x3ff)
	// right = uint16((r.Sensors >> 20) & 0x3ff)
	// return
}

func (r *Report) DecodeLEDs() (onboard, left, right, ir bool) {
	onboard = (r.LEDs & 0x01) == 1
	left = ((r.LEDs >> 1) & 0x01) == 1
	right = ((r.LEDs >> 2) & 0x01) == 1
	ir = ((r.LEDs >> 3) & 0x01) == 1
	return
}

func (r *Report) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("encoder_left", "wire", 32),
		vcd.NewVariable("encoder_right", "wire", 32),
		vcd.NewVariable("motor_power_left", "wire", 16),
		vcd.NewVariable("motor_power_right", "wire", 16),
		vcd.NewVariable("speed_measured_left", "wire", 32),
		vcd.NewVariable("speed_measured_right", "wire", 32),
		vcd.NewVariable("speed_setpoint_left", "wire", 32),
		vcd.NewVariable("speed_setpoint_right", "wire", 32),
		vcd.NewVariable("position_distance", "wire", 32),
		vcd.NewVariable("position_theta", "wire", 32),
		vcd.NewVariable("rtc_now", "wire", 32),
		vcd.NewVariable("plan_type", "wire", 8),
		vcd.NewVariable("plan_state", "wire", 8),
		vcd.NewVariable("plan_power_left", "wire", 16),
		vcd.NewVariable("plan_power_right", "wire", 16),
		vcd.NewVariable("plan_speed_left", "wire", 32),
		vcd.NewVariable("plan_speed_right", "wire", 32),
		vcd.NewVariable("plan_linear_distance", "wire", 32),
		vcd.NewVariable("plan_linear_stop", "wire", 1),
		vcd.NewVariable("plan_rotational_dtheta", "wire", 32),
	}
}

func (r *Report) Symbols() map[string]string {
	boolMap := map[bool]string{
		true:  "1",
		false: "0",
	}

	result := map[string]string{
		"encoder_left":         fmt.Sprint(uint32(r.EncoderLeft)),
		"encoder_right":        fmt.Sprint(uint32(r.EncoderRight)),
		"motor_power_left":     fmt.Sprint(uint16(r.MotorPowerLeft)),
		"motor_power_right":    fmt.Sprint(uint16(r.MotorPowerRight)),
		"speed_measured_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedMeasuredLeft))),
		"speed_measured_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedMeasuredRight))),
		"speed_setpoint_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedSetpointLeft))),
		"speed_setpoint_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedSetpointRight))),
		"position_distance":    fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.PositionDistance))),
		"position_theta":       fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.PositionTheta))),
		"rtc_now":              fmt.Sprint(r.RTCMicros),
		"plan_type":            fmt.Sprint(uint8(r.Plan.Type)),
		"plan_state":           fmt.Sprint(uint8(r.Plan.State)),
	}

	decodedPlan := r.Plan.Decode()
	switch p := decodedPlan.(type) {
	case PlanFixedPower:
		result["plan_power_left"] = fmt.Sprint(uint16(p.Left))
		result["plan_power_right"] = fmt.Sprint(uint16(p.Right))
	case PlanFixedSpeed:
		result["plan_speed_left"] = fmt.Sprint(*(*uint32)(unsafe.Pointer(&p.Left)))
		result["plan_speed_right"] = fmt.Sprint(*(*uint32)(unsafe.Pointer(&p.Right)))
	case PlanLinearMotion:
		result["plan_linear_distance"] = fmt.Sprint(*(*uint32)(unsafe.Pointer(&p.Distance)))
		result["plan_linear_stop"] = boolMap[p.Stop]
	case PlanRotationalMotion:
		result["plan_rotational_dtheta"] = fmt.Sprint(*(*uint32)(unsafe.Pointer(&p.DTheta)))
	}

	return result
}
