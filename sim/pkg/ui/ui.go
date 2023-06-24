package ui

import (
	"context"
	"fmt"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type window interface {
	init()
	draw()
}

type UI struct {
	sim     *sim.Sim
	backend imgui.Backend
	windows []window
}

func New(sim *sim.Sim) *UI {
	backend := imgui.CreateBackend(imgui.NewGLFWBackend())

	ui := &UI{
		sim:     sim,
		backend: backend,
		windows: []window{
			newToolbarWindow(sim),
			newMouseWindow(sim),
			newSymbolsWindow(sim),
		},
	}

	backend.SetBgColor(imgui.NewVec4(0.45, 0.55, 0.6, 1.0))
	backend.SetAfterCreateContextHook(ui.init)
	backend.CreateWindow("Mouse Simulator", 1280, 768, imgui.GLFWWindowFlags(0))
	backend.SetTargetFPS(60)

	imgui.CurrentIO().Fonts().AddFontFromFileTTF("fonts/DroidSans.ttf", 24)
	imgui.CurrentIO().SetConfigFlags(imgui.ConfigFlagsDockingEnable)
	imgui.StyleColorsClassic()

	return ui
}

func (ui *UI) init() {
	for _, window := range ui.windows {
		window.init()
	}
}

func (ui *UI) Run(ctx context.Context) {
	firstRun := true

	go func() {
		<-ctx.Done()
		ui.backend.SetShouldClose(true)
	}()

	ui.backend.Run(func() {
		var dockspaceFlags imgui.WindowFlags = imgui.WindowFlagsNoDocking |
			imgui.WindowFlagsNoTitleBar |
			imgui.WindowFlagsNoCollapse |
			imgui.WindowFlagsNoResize |
			imgui.WindowFlagsNoMove |
			imgui.WindowFlagsNoBringToFrontOnFocus |
			imgui.WindowFlagsNoNavFocus

		vp := imgui.MainViewport()
		imgui.SetNextWindowPos(vp.Pos().Add(imgui.NewVec2(0, 48)))
		imgui.SetNextWindowSize(vp.Size().Sub(imgui.NewVec2(0, 48)))
		imgui.SetNextWindowViewport(vp.ID())
		imgui.PushStyleVarVec2(imgui.StyleVarWindowPadding, imgui.NewVec2(0, 0))
		imgui.PushStyleVarFloat(imgui.StyleVarWindowRounding, 0)
		imgui.PushStyleVarFloat(imgui.StyleVarWindowBorderSize, 0)
		imgui.BeginV("Dockspace", nil, dockspaceFlags)
		imgui.PopStyleVarV(3)
		dockID := imgui.IDStr("Dockspace")
		imgui.DockSpace(dockID)
		imgui.End()

		if firstRun {
			firstRun = false
			fmt.Println("Setting layout...")
			imgui.InternalDockBuilderRemoveNode(dockID)
			imgui.InternalDockBuilderAddNodeV(dockID, imgui.DockNodeFlagsDockSpace)
			imgui.InternalDockBuilderSetNodeSize(dockID, vp.Size().Sub(imgui.NewVec2(0, 48)))
			dockRight := imgui.InternalDockBuilderSplitNode(dockID, imgui.DirRight, 0.3, nil, &dockID)
			imgui.InternalDockBuilderDockWindow("Symbols", dockRight)
			imgui.InternalDockBuilderDockWindow("Mouse", dockID)
			imgui.InternalDockBuilderFinish(dockID)
		}

		for _, window := range ui.windows {
			window.draw()
		}

		ui.backend.Refresh()
	})
}
