package mouse

import (
	"fmt"
	"unsafe"

	"github.com/elamre/vcd"
)

type Report struct {
	BatteryVolts          uint8
	Mode                  uint8
	Sensors               uint32
	LEDs                  uint8
	EncoderLeft           uint16
	EncoderRight          uint16
	MotorPowerLeft        uint16
	MotorPowerRight       uint16
	MotorForward          uint8
	SpeedMeasuredLeft     float32
	SpeedMeasuredRight    float32
	SpeedSetpointLeft     float32
	SpeedSetpointRight    float32
	PositionMeasuredLeft  float32
	PositionMeasuredRight float32
	PositionSetpointLeft  float32
	PositionSetpointRight float32
	RTCMicros             uint32
	Padding               [2]uint8
}

func (r *Report) DecodeSensors() (left, center, right uint16) {
	left = uint16(r.Sensors & 0x3ff)
	center = uint16((r.Sensors >> 10) & 0x3ff)
	right = uint16((r.Sensors >> 20) & 0x3ff)
	return
}

func (r *Report) DecodeLEDs() (onboard, left, right, ir bool) {
	onboard = (r.LEDs & 0x01) == 1
	left = ((r.LEDs >> 1) & 0x01) == 1
	right = ((r.LEDs >> 2) & 0x01) == 1
	ir = ((r.LEDs >> 3) & 0x01) == 1
	return
}

func (r *Report) DecodeMotorForward() (left, right bool) {
	left = (r.MotorForward & 0x01) == 1
	right = ((r.MotorForward >> 1) & 0x01) == 1
	return
}

func (r *Report) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("adc_battery_voltage", "wire", 10),
		vcd.NewVariable("mode_active", "wire", 8),
		vcd.NewVariable("adc_sensor_center", "wire", 10),
		vcd.NewVariable("adc_sensor_left", "wire", 10),
		vcd.NewVariable("adc_sensor_right", "wire", 10),
		vcd.NewVariable("encoder_left", "wire", 16),
		vcd.NewVariable("encoder_right", "wire", 16),
		vcd.NewVariable("motor_power_left", "wire", 16),
		vcd.NewVariable("motor_power_right", "wire", 16),
		vcd.NewVariable("motor_forward_left", "wire", 1),
		vcd.NewVariable("motor_forward_right", "wire", 1),
		vcd.NewVariable("speed_measured_left", "wire", 32),
		vcd.NewVariable("speed_measured_right", "wire", 32),
		vcd.NewVariable("speed_setpoint_left", "wire", 32),
		vcd.NewVariable("speed_setpoint_right", "wire", 32),
		vcd.NewVariable("position_measured_left", "wire", 32),
		vcd.NewVariable("position_measured_right", "wire", 32),
		vcd.NewVariable("position_setpoint_left", "wire", 32),
		vcd.NewVariable("position_setpoint_right", "wire", 32),
		vcd.NewVariable("rtc_now", "wire", 32),
	}
}

func (r *Report) Symbols() map[string]string {
	sl, sc, sr := r.DecodeSensors()
	lf, rf := r.DecodeMotorForward()

	bool2Int := map[bool]int{
		true:  1,
		false: 0,
	}

	return map[string]string{
		"adc_battery_voltage":     fmt.Sprint(r.BatteryVolts * 2),
		"mode_active":             fmt.Sprint(r.Mode),
		"adc_sensor_center":       fmt.Sprint(sc),
		"adc_sensor_left":         fmt.Sprint(sl),
		"adc_sensor_right":        fmt.Sprint(sr),
		"encoder_left":            fmt.Sprint(r.EncoderLeft),
		"encoder_right":           fmt.Sprint(r.EncoderRight),
		"motor_power_left":        fmt.Sprint(r.MotorPowerLeft),
		"motor_power_right":       fmt.Sprint(r.MotorPowerRight),
		"motor_forward_left":      fmt.Sprint(bool2Int[lf]),
		"motor_forward_right":     fmt.Sprint(bool2Int[rf]),
		"speed_measured_left":     fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedMeasuredLeft))),
		"speed_measured_right":    fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedMeasuredRight))),
		"speed_setpoint_left":     fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedSetpointLeft))),
		"speed_setpoint_right":    fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedSetpointRight))),
		"position_measured_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.PositionMeasuredLeft))),
		"position_measured_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.PositionMeasuredRight))),
		"position_setpoint_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.PositionSetpointLeft))),
		"position_setpoint_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.PositionSetpointRight))),
		"rtc_now":                 fmt.Sprint(r.RTCMicros),
	}
}
