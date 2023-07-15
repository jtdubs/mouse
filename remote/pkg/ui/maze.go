package ui

import (
	"fmt"
	"math"

	"github.com/jtdubs/mouse/remote/pkg/mouse"

	imgui "github.com/AllenDang/cimgui-go"
)

type mazeWindow struct {
	mouse *mouse.Mouse
}

func newMazeWindow(mouse *mouse.Mouse) *mazeWindow {
	return &mazeWindow{
		mouse: mouse,
	}
}

func (w *mazeWindow) init() {}

func (w *mazeWindow) draw() {
	gridWidth := len(w.mouse.Maze.Cells)
	gridHeight := 0
	if gridWidth > 0 {
		gridHeight = len(w.mouse.Maze.Cells[0])
	}

	gridDim := imgui.NewVec2(float32(gridWidth), float32(gridHeight))

	if !imgui.Begin("Maze") {
		return
	}

	canvasSizePx := imgui.ContentRegionAvail()
	cellSizePx := float32(math.Min(float64(canvasSizePx.X/gridDim.X), float64(canvasSizePx.Y/gridDim.Y)))
	mazeSizePx := imgui.NewVec2(cellSizePx*gridDim.X, cellSizePx*gridDim.Y)
	marginPx := canvasSizePx.Sub(mazeSizePx).Div(2.0)
	mazeOriginPx := imgui.CursorScreenPos().Add(imgui.NewVec2(0, canvasSizePx.Y)).Add(imgui.NewVec2(marginPx.X, -marginPx.Y))

	imgui.Dummy(canvasSizePx)
	if imgui.BeginPopupContextItem() {
		if imgui.SelectableBool("Refresh") {
			w.mouse.SendCommand(mouse.NewSendMazeCommand())
		}
		imgui.EndPopup()
	}

	drawList := imgui.WindowDrawList()
	for j, col := range w.mouse.Maze.Cells {
		for i, cell := range col {
			cellOriginPx := mazeOriginPx.Add(imgui.NewVec2(float32(j)*cellSizePx, -float32(i)*cellSizePx))
			if cell.Visited {
				drawList.AddRectFilled(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(cellSizePx, -cellSizePx)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.2, 0.2, 0.2, 1)))
			}
			if cell.North {
				drawList.AddLineV(cellOriginPx.Add(imgui.NewVec2(0, -cellSizePx)), cellOriginPx.Add(imgui.NewVec2(cellSizePx, -cellSizePx)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if cell.East {
				drawList.AddLineV(cellOriginPx.Add(imgui.NewVec2(cellSizePx, 0)), cellOriginPx.Add(imgui.NewVec2(cellSizePx, -cellSizePx)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if cell.South {
				drawList.AddLineV(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(cellSizePx, 0)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if cell.West {
				drawList.AddLineV(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(0, -cellSizePx)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if cell.Distance != 0xFF {
				label := fmt.Sprintf("%02X", cell.Distance)
				var fontSize float32 = 24.0
				var labelSize imgui.Vec2
				for {
					labelSize = imgui.CurrentFont().CalcTextSizeA(fontSize, 1000.0, 1000.0, label)
					if labelSize.X < cellSizePx && labelSize.Y < cellSizePx {
						break
					}
					fontSize--
				}
				drawList.AddTextFontPtr(
					imgui.CurrentFont(),
					fontSize,
					cellOriginPx.Add(imgui.NewVec2(cellSizePx/2, -cellSizePx/2)).Add(imgui.NewVec2(-labelSize.X/2, -labelSize.Y/2)),
					imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)),
					label)
			}
		}
	}

	pathStack, nextStack := w.mouse.Explore.Read()
	for _, cell := range nextStack {
		x, y := cell>>4, cell&0x0F
		cellOriginPx := mazeOriginPx.Add(imgui.NewVec2(float32(x)*cellSizePx, -float32(y)*cellSizePx))
		drawList.AddCircleFilled(cellOriginPx.Add(imgui.NewVec2(cellSizePx/2, -cellSizePx/2)), cellSizePx/4, imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.3, 0.1, 0.1, 1)))
	}
	for _, cell := range pathStack {
		x, y := cell>>4, cell&0x0F
		cellOriginPx := mazeOriginPx.Add(imgui.NewVec2(float32(x)*cellSizePx, -float32(y)*cellSizePx))
		drawList.AddCircleFilled(cellOriginPx.Add(imgui.NewVec2(cellSizePx/2, -cellSizePx/2)), cellSizePx/8, imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.0, 0.0, 0.0, 1)))
	}

	imgui.End()
}
