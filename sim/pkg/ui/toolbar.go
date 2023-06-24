package ui

import (
	"fmt"
	"path/filepath"
	"strings"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

func mustLoad(image string) *imgui.Texture {
	img, err := imgui.LoadImage(image)
	if err != nil {
		panic(err)
	}
	tex := imgui.NewTextureFromRgba(img)
	fmt.Printf("Loaded %s (%vx%v) as %d\n", image, tex.Width, tex.Height, tex.ID())
	return tex
}

type toolbarWindow struct {
	sim      *sim.Sim
	textures map[string]*imgui.Texture
}

func newToolbarWindow(sim *sim.Sim) *toolbarWindow {
	return &toolbarWindow{
		sim:      sim,
		textures: map[string]*imgui.Texture{},
	}
}

func (s *toolbarWindow) init() {
	files, _ := filepath.Glob("icons/*.png")
	for _, file := range files {
		name := strings.TrimSuffix(filepath.Base(file), filepath.Ext(file))
		s.textures[name] = mustLoad(file)
	}
}

func (s *toolbarWindow) toolbarButton(name string, icon string) bool {
	return imgui.ImageButtonV(name, s.textures[icon].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}

func (s *toolbarWindow) drawToolbar() {
	switch s.sim.State {
	case sim.Crashed:
		imgui.Text("Crashed")
	case sim.Done:
		imgui.Text("Done")
	default:
		imgui.BeginDisabledV(s.sim.State == sim.Running)
		if s.toolbarButton("SimPlay", "play") {
			s.sim.SetRunning(true)
		}
		imgui.EndDisabled()
		imgui.SameLine()
		imgui.BeginDisabledV(s.sim.State == sim.Paused)
		if s.toolbarButton("SimPause", "pause") {
			s.sim.SetRunning(false)
		}
		imgui.EndDisabled()
	}
	imgui.SameLineV(0, 20)

	imgui.BeginDisabledV(s.sim.Recording)
	if s.toolbarButton("VCDRecord", "video-outline") {
		s.sim.SetRecording(true)
	}
	imgui.EndDisabled()
	imgui.SameLine()
	imgui.BeginDisabledV(!s.sim.Recording)
	if s.toolbarButton("VCDStop", "video-off-outline") {
		s.sim.SetRecording(false)
	}
	imgui.EndDisabled()
}

func (s *toolbarWindow) draw(dock imgui.ID) {
	var toolbarFlags imgui.WindowFlags = imgui.WindowFlagsNoDocking |
		imgui.WindowFlagsNoTitleBar |
		imgui.WindowFlagsNoResize |
		imgui.WindowFlagsNoMove |
		imgui.WindowFlagsNoScrollbar |
		imgui.WindowFlagsNoSavedSettings

	vp := imgui.MainViewport()
	imgui.SetNextWindowPos(vp.Pos())
	imgui.SetNextWindowSize(imgui.NewVec2(vp.Size().X, 48))
	imgui.SetNextWindowViewport(vp.ID())
	imgui.PushStyleVarFloat(imgui.StyleVarWindowBorderSize, 0)
	imgui.BeginV("TOOLBAR", nil, toolbarFlags)
	imgui.PopStyleVar()
	s.drawToolbar()
	imgui.End()
}
