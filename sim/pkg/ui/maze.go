package ui

import (
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

	topLeft := imgui.CursorScreenPos()
	size := imgui.ContentRegionAvail()

	drawList := imgui.WindowDrawList()

	dimX := float32(len(s.sim.Maze.Posts[0]))
	dimY := float32(len(s.sim.Maze.Posts))

	cellSize := float32(math.Min(float64(size.X/dimX), float64(size.Y/dimY)))

	cellOffsetX := (size.X - (cellSize * (dimX - 1))) / 2.0
	cellOffsetY := (size.Y - (cellSize * (dimY - 1))) / 2.0

	for i, row := range s.sim.Maze.Posts {
		for j, post := range row {
			x := topLeft.X + cellOffsetX + float32(j)*cellSize
			y := topLeft.Y + cellOffsetY + float32(i)*cellSize
			if post.South {
				drawList.AddLineV(imgui.NewVec2(x, y), imgui.NewVec2(x, y+cellSize), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if post.East {
				drawList.AddLineV(imgui.NewVec2(x, y), imgui.NewVec2(x+cellSize, y), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
		}
	}

	imgui.End()
}
