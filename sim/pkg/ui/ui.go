package ui

import (
	"context"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type window interface {
	init()
	draw(dock imgui.ID)
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
		},
	}

	backend.SetBgColor(imgui.NewVec4(0.45, 0.55, 0.6, 1.0))
	backend.SetAfterCreateContextHook(ui.init)
	backend.CreateWindow("Mouse Simulator", 1024, 768, imgui.GLFWWindowFlags(0))
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
		imgui.BeginV("Master Dockspace", nil, dockspaceFlags)
		dockID := imgui.IDStr("Dockspace")
		imgui.DockSpace(dockID)
		imgui.End()
		imgui.PopStyleVarV(3)

		for _, window := range ui.windows {
			window.draw(dockID)
		}

		ui.backend.Refresh()
	})
}
