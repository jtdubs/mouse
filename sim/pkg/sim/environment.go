package sim

import "math"

// Fundamental constants
const (
	GridSize                float64 = 18.0 // in mm
	WheelBase               float64 = 90.0 // in mm
	WheelDiameter           float64 = 32.0 // in mm
	EncoderTicksPerRotation int     = 240
)

// Derived constants
var (
	WheelCircumference     float64 = math.Pi * WheelDiameter
	DistancePerEncoderTick float64 = WheelCircumference / float64(EncoderTicksPerRotation)
	EncoderTickDtheta      float64 = DistancePerEncoderTick / WheelBase
	EncoderTickDx          float64 = (WheelBase / 2.0) * (1.0 - math.Cos(EncoderTickDtheta))
	EncoderTickDy          float64 = (WheelBase / 2.0) * math.Sin(EncoderTickDtheta)
)

type Environment struct {
	MouseX, MouseY float64 // center of the axle, in mm from the maze center
	MouseAngle     float64 // in radians, 0 is the positive x-axis (east)
	encoderChan    <-chan EncoderTickEvent
}

func NewEnvironment(mazeWidth, mazeHeight int, encoderChan <-chan EncoderTickEvent) *Environment {
	// start in the southwest corner, facing north
	return &Environment{
		MouseX:      -GridSize * ((float64(mazeWidth) / 2.0) - 0.5),
		MouseY:      -GridSize * ((float64(mazeHeight) / 2.0) - 0.5),
		MouseAngle:  math.Pi / 2.0,
		encoderChan: encoderChan,
	}
}

func (e *Environment) Init() {
	go func() {
		for ev := range e.encoderChan {
			if ev.Left {
				e.LeftEncoderTick(ev.Forward)
			} else {
				e.RightEncoderTick(ev.Forward)
			}
		}
	}()
}

func (e *Environment) dx() float64 {
	return (EncoderTickDx * math.Cos(e.MouseAngle)) - (EncoderTickDy * math.Sin(e.MouseAngle))
}

func (e *Environment) dy() float64 {
	return (EncoderTickDx * math.Sin(e.MouseAngle)) - (EncoderTickDy * math.Cos(e.MouseAngle))
}

func (e *Environment) LeftEncoderTick(forward bool) {
	if forward {
		e.MouseX += e.dx()
		e.MouseY += e.dy()
		e.MouseAngle -= EncoderTickDtheta
	} else {
		e.MouseX += e.dx()
		e.MouseY -= e.dy()
		e.MouseAngle += EncoderTickDtheta
	}
}

func (e *Environment) RightEncoderTick(forward bool) {
	if forward {
		e.MouseX -= e.dx()
		e.MouseY += e.dy()
		e.MouseAngle += EncoderTickDtheta
	} else {
		e.MouseX -= e.dx()
		e.MouseY -= e.dy()
		e.MouseAngle -= EncoderTickDtheta
	}
}
