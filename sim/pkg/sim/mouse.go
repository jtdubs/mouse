package sim

/*
#cgo pkg-config: simavr simavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <avr_timer.h>
#include <parts/uart_pty.h>
#include <sim_vcd_file.h>
#include <sim_avr.h>
#include <sim_elf.h>
#include <sim_gdb.h>
*/
import "C"

type Mouse struct {
	avr                                   *C.avr_t
	LEDs                                  *LEDs
	Battery                               *Battery
	FunctionSelector                      *FunctionSelector
	LeftSensor, CenterSensor, RightSensor *Sensor
	LeftMotor, RightMotor                 *Motor
	Environment                           *Environment
}

func NewMouse(avr *C.avr_t) *Mouse {
	encoderChan := make(chan EncoderTickEvent, 100)

	return &Mouse{
		avr:              avr,
		LEDs:             NewLEDs(avr),
		Battery:          NewBattery(avr, C.ADC_IRQ_ADC7),
		FunctionSelector: NewFunctionSelect(avr, C.ADC_IRQ_ADC6),
		LeftMotor:        NewMotor(avr, true, encoderChan),
		RightMotor:       NewMotor(avr, false, encoderChan),
		LeftSensor:       NewSensor(avr, "SENSOR_LEFT", C.ADC_IRQ_ADC2),
		CenterSensor:     NewSensor(avr, "SENSOR_CENTER", C.ADC_IRQ_ADC1),
		RightSensor:      NewSensor(avr, "SENSOR_RIGHT", C.ADC_IRQ_ADC0),
		Environment:      NewEnvironment(16, 16, encoderChan),
	}
}

func (m *Mouse) Init() {
	m.LEDs.Init()
	m.Battery.Init()
	m.FunctionSelector.Init()
	m.LeftMotor.Init()
	m.RightMotor.Init()
	m.LeftSensor.Init()
	m.CenterSensor.Init()
	m.RightSensor.Init()
	m.Environment.Init()
}
