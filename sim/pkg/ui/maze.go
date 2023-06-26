package ui

import (
	"fmt"
	"math"
	"strings"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type mazeWindow struct {
	sim       *sim.Sim
	m         *sim.Mouse
	mazeList  string
	mazeIndex int32
}

func newMazeWindow(sim *sim.Sim) *mazeWindow {
	return &mazeWindow{
		sim: sim,
		m:   sim.Mouse,
	}
}

func (s *mazeWindow) init() {
	s.mazeList = strings.Join(s.sim.Maze.ListMazes(), "\x00") + "\x00\x00"
}

func (s *mazeWindow) draw() {
	imgui.Begin("Maze")

	imgui.Text("Maze:")
	imgui.SameLine()
	if imgui.ComboStr("##MAZE", &s.mazeIndex, s.mazeList) {
		s.sim.Maze.Load(s.sim.Maze.ListMazes()[s.mazeIndex])
	}
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("(x=%02.2f, y=%02.2f, θ=%02.2f)", s.m.Environment.MouseX, s.m.Environment.MouseY, s.m.Environment.MouseAngle*180.0/math.Pi))

	canvasSizePx := imgui.ContentRegionAvail()
	postDim := imgui.NewVec2(float32(len(s.sim.Maze.Posts[0])), float32(len(s.sim.Maze.Posts)))
	gridDim := postDim.Sub(imgui.NewVec2(1, 1))
	cellSizePx := float32(math.Min(float64(canvasSizePx.X/gridDim.X), float64(canvasSizePx.Y/gridDim.Y)))
	mazeSizePx := imgui.NewVec2(cellSizePx*gridDim.X, cellSizePx*gridDim.Y)
	mmSizePx := cellSizePx / 180.0
	marginPx := canvasSizePx.Sub(mazeSizePx).Div(2.0)
	mazeOriginPx := imgui.CursorScreenPos().Add(imgui.NewVec2(0, canvasSizePx.Y)).Add(imgui.NewVec2(marginPx.X, -marginPx.Y))

	drawList := imgui.WindowDrawList()
	for i, row := range s.sim.Maze.Posts {
		for j, post := range row {
			cellOriginPx := mazeOriginPx.Add(imgui.NewVec2(float32(j)*cellSizePx, -float32(i)*cellSizePx))
			if post.North {
				drawList.AddLineV(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(0, -cellSizePx)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if post.East {
				drawList.AddLineV(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(cellSizePx, 0)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
		}
	}

	x, y, theta := float32(s.m.Environment.MouseX), float32(s.m.Environment.MouseY), s.m.Environment.MouseAngle
	centerPx := mazeOriginPx.Add(imgui.NewVec2(x, -y).Mul(mmSizePx))
	mouseRadius := 45.0 * mmSizePx

	drawList.AddTriangleV(
		centerPx.Add(imgui.NewVec2(float32(math.Cos(theta+0.0*math.Pi/3.0)), float32(-math.Sin(theta+0.0*math.Pi/3.0))).Mul(mouseRadius)),
		centerPx.Add(imgui.NewVec2(float32(math.Cos(theta+2.0*math.Pi/3.0)), float32(-math.Sin(theta+2.0*math.Pi/3.0))).Mul(mouseRadius)),
		centerPx.Add(imgui.NewVec2(float32(math.Cos(theta+4.0*math.Pi/3.0)), float32(-math.Sin(theta+4.0*math.Pi/3.0))).Mul(mouseRadius)),
		imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.5, 1, 0.5, 1)),
		2.0,
	)

	imgui.End()
}
