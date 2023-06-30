package mouse

import (
	"fmt"
	"unsafe"

	"github.com/elamre/vcd"
)

type Report struct {
	BatteryVolts       uint8
	Sensors            uint32
	LEDs               uint8
	EncoderLeft        int32
	EncoderRight       int32
	MotorPowerLeft     int16
	MotorPowerRight    int16
	SpeedMeasuredLeft  float32
	SpeedMeasuredRight float32
	SpeedSetpointLeft  float32
	SpeedSetpointRight float32
	RTCMicros          uint32
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

func (r *Report) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("adc_battery_voltage", "wire", 10),
		vcd.NewVariable("adc_sensor_center", "wire", 10),
		vcd.NewVariable("adc_sensor_left", "wire", 10),
		vcd.NewVariable("adc_sensor_right", "wire", 10),
		vcd.NewVariable("encoder_left", "wire", 32),
		vcd.NewVariable("encoder_right", "wire", 32),
		vcd.NewVariable("motor_power_left", "wire", 16),
		vcd.NewVariable("motor_power_right", "wire", 16),
		vcd.NewVariable("speed_measured_left", "wire", 32),
		vcd.NewVariable("speed_measured_right", "wire", 32),
		vcd.NewVariable("speed_setpoint_left", "wire", 32),
		vcd.NewVariable("speed_setpoint_right", "wire", 32),
		// vcd.NewVariable("speed_kp", "wire", 32),
		// vcd.NewVariable("speed_ki", "wire", 32),
		// vcd.NewVariable("speed_kd", "wire", 32),
		vcd.NewVariable("rtc_now", "wire", 32),
	}
}

func (r *Report) Symbols() map[string]string {
	sl, sc, sr := r.DecodeSensors()

	return map[string]string{
		"adc_battery_voltage":  fmt.Sprint(r.BatteryVolts * 2),
		"adc_sensor_center":    fmt.Sprint(sc),
		"adc_sensor_left":      fmt.Sprint(sl),
		"adc_sensor_right":     fmt.Sprint(sr),
		"encoder_left":         fmt.Sprint(r.EncoderLeft),
		"encoder_right":        fmt.Sprint(r.EncoderRight),
		"motor_power_left":     fmt.Sprint(r.MotorPowerLeft),
		"motor_power_right":    fmt.Sprint(r.MotorPowerRight),
		"speed_measured_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedMeasuredLeft))),
		"speed_measured_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedMeasuredRight))),
		"speed_setpoint_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedSetpointLeft))),
		"speed_setpoint_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedSetpointRight))),
		// "speed_kp":             fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedKp))),
		// "speed_ki":             fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedKi))),
		// "speed_kd":             fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.SpeedKd))),
		"rtc_now": fmt.Sprint(r.RTCMicros),
	}
}
