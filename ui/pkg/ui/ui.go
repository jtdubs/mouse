package ui

import (
	"context"

	"github.com/jtdubs/mouse/ui/pkg/mouse"

	imgui "github.com/AllenDang/cimgui-go"
)

type window interface {
	draw()
}

type UI struct {
	mouse   *mouse.Mouse
	backend imgui.Backend
	windows []window
}

func New(mouse *mouse.Mouse) *UI {
	backend := imgui.CreateBackend(imgui.NewGLFWBackend())
	backend.SetBgColor(imgui.NewVec4(0.45, 0.55, 0.6, 1.0))
	backend.CreateWindow("Mouse Remote", 1024, 1024, 0)
	backend.SetTargetFPS(60)

	imgui.CurrentIO().Fonts().AddFontFromFileTTF("fonts/DroidSans.ttf", 24)
	imgui.CurrentIO().SetConfigFlags(imgui.ConfigFlagsDockingEnable)

	return &UI{
		mouse:   mouse,
		backend: backend,
		windows: []window{
			newSerialWindow(mouse),
		},
	}
}

func (ui *UI) Run(ctx context.Context) {
	go func() {
		<-ctx.Done()
		ui.backend.SetShouldClose(true)
	}()
	ui.backend.Run(func() {
		dockspace := imgui.DockSpaceOverViewport()
		for _, window := range ui.windows {
			imgui.SetNextWindowDockIDV(dockspace, imgui.CondFirstUseEver)
			window.draw()
		}
		ui.backend.Refresh()
	})
}
