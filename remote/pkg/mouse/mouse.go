package mouse

import (
	"bytes"
	"context"
	"encoding/binary"
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"github.com/elamre/vcd"
	"go.bug.st/serial"
)

var (
	portName   = flag.String("port", "/dev/ttyNano", "Serial port")
	baudRate   = flag.Int("baud", 115200, "Baud rate")
	scrollback = flag.Int("scrollback", 100000, "Number of lines to keep in the scrollback buffer")
)

type Mouse struct {
	Recording    bool
	Updating     bool
	Maze         *Maze
	Explore      *Explore
	status       string
	portOpen     bool
	serialBuffer []byte
	frameBuffer  []byte
	readState    readState
	readLength   uint8
	readIndex    uint8
	checksum     uint8
	index        int
	messages     *Ring[string]
	reports      map[ReportKey]Report
	reportMutex  sync.Mutex
	sendChan     chan Command
	vcd          vcd.VcdWriter
	vcdTime      uint64
}

func New() *Mouse {
	return &Mouse{
		Recording:    false,
		Updating:     true,
		Maze:         &Maze{},
		Explore:      &Explore{},
		status:       "Closed",
		portOpen:     false,
		serialBuffer: make([]byte, 256),
		frameBuffer:  make([]byte, 256),
		readState:    Idle,
		readLength:   0,
		readIndex:    0,
		index:        0,
		messages:     NewRing[string](*scrollback),
		reports:      make(map[ReportKey]Report),
		reportMutex:  sync.Mutex{},
		sendChan:     make(chan Command, 1),
		vcd:          vcd.VcdWriter{},
		vcdTime:      0,
	}
}

func (m *Mouse) Report(key ReportKey) (r Report, ok bool) {
	m.reportMutex.Lock()
	defer m.reportMutex.Unlock()
	r, ok = m.reports[key]
	return
}

func (m *Mouse) SetRecording(recording bool) {
	if recording == m.Recording {
		return
	}

	if recording {
		var err error
		m.vcd, err = vcd.New("mouse.vcd", "1us")
		if err != nil {
			log.Printf("Failed to enable recording: %v", err)
			return
		}
		m.vcd.RegisterVariableList("mouse", Report{}.Variables())
		m.vcdTime = 0
	} else {
		m.vcd.Close()
	}

	m.Recording = recording
}

func (m *Mouse) SetUpdating(updating bool) {
	m.Updating = updating
}

func (m *Mouse) Run(ctx context.Context) {
	signalChan := make(chan os.Signal, 1)
	signal.Notify(signalChan, syscall.SIGHUP, syscall.SIGCONT)

	defer func() {
		m.status = "Canceled"
		m.portOpen = false
	}()

	for {
		port := m.open(ctx, signalChan)
		if port == nil {
			return
		}
		if !m.read(ctx, port, signalChan) {
			return
		}
	}
}

func (m *Mouse) PortName() string {
	return *portName
}

func (m *Mouse) Open() bool {
	return m.portOpen
}

func (m *Mouse) Status() string {
	return m.status
}

func (m *Mouse) ForEachMessage(f func(string)) {
	m.messages.ForEach(f)
}

func (m *Mouse) Clear() {
	m.messages.Clear()
}

func (m *Mouse) SendCommand(c Command) {
	if m.portOpen {
		m.sendChan <- c
	}
}

func (m *Mouse) open(ctx context.Context, signalChan <-chan os.Signal) serial.Port {
	m.portOpen = false
	m.status = "Polling"

	for {
		port, err := serial.Open(*portName, &serial.Mode{BaudRate: *baudRate})
		if err == nil {
			port.SetReadTimeout(100 * time.Millisecond)
			m.portOpen = true
			m.status = "Open"
			return port
		}

		select {
		case <-ctx.Done():
			return nil

		case sig := <-signalChan:
			if sig == syscall.SIGHUP {
				if !m.sleep(ctx, signalChan) {
					return nil
				}
			}
			m.status = "Polling"

		case <-time.After(1 * time.Second):
			if len(m.status) > 11 {
				m.status = "Polling."
			} else {
				m.status = m.status + "."
			}
		}
	}
}

func (m *Mouse) read(ctx context.Context, port serial.Port, signalChan <-chan os.Signal) bool {
	defer port.Close()

	closedChan := make(chan struct{})

	go func() {
		defer close(closedChan)
		for {
			n, err := port.Read(m.serialBuffer)
			if err != nil {
				return
			}
			for _, b := range m.serialBuffer[:n] {
				m.receiveByte(b)
			}
		}
	}()

	for {
		select {
		case <-ctx.Done():
			port.Close()
			m.portOpen = false
			return false
		case <-signalChan:
			port.Close()
			m.portOpen = false
			if !m.sleep(ctx, signalChan) {
				return false
			}
			return true
		case cmd := <-m.sendChan:
			var buf bytes.Buffer
			buf.WriteByte(0x02)
			buf.WriteByte(byte(binary.Size(cmd)))
			binary.Write(&buf, binary.LittleEndian, cmd)
			var checksum byte
			for _, b := range buf.Bytes()[2:] {
				checksum += b
			}
			buf.WriteByte(-checksum)
			m.messages.Add(fmt.Sprintf("Sending %q", hex.EncodeToString(buf.Bytes())))
			port.Write(buf.Bytes())
		case <-closedChan:
			return true
		}
	}
}

func (m *Mouse) sleep(ctx context.Context, signalChan <-chan os.Signal) bool {
	m.status = "Paused"
	for {
		select {
		case <-ctx.Done():
			return false

		case sig := <-signalChan:
			if sig == syscall.SIGCONT {
				return true
			}
		}
	}
}

type readState uint8

const (
	Idle readState = iota
	Length
	Data
	Checksum
)

func (m *Mouse) receiveByte(value byte) {
	switch m.readState {
	case Idle:
		if value == 0x02 {
			m.readState = Length
			m.checksum = 0
			m.readIndex = 0
		}
	case Length:
		m.readLength = value
		m.readState = Data
		if value == 0 || value > 64 {
			m.readState = Idle
		}
	case Data:
		m.frameBuffer[m.readIndex] = value
		m.checksum += value
		m.readIndex++
		if m.readIndex == m.readLength {
			m.readState = Checksum
		}
	case Checksum:
		m.readState = Idle
		m.checksum += value
		if m.checksum == 0 {
			r, err := ReadReport(bytes.NewReader(m.frameBuffer[0:m.readLength]))
			if err != nil {
				m.messages.Add(fmt.Sprintf("Error reading report: %v", err))
			}

			if m.Updating {
				m.reportMutex.Lock()
				m.reports[ReportKeyLatest] = r
				if r.Key() == ReportKeyMaze {
					for _, u := range r.Body.(MazeReport).Updates {
						m.Maze.Update(u)
					}
				} else if r.Key() == ReportKeyExplore {
					for _, u := range r.Body.(ExploreReport).Updates {
						m.Explore.Update(u)
					}
				} else {
					m.reports[r.Key()] = r
				}
				m.reportMutex.Unlock()
			}

			if m.Recording {
				now := uint64(r.Header.RTCMicros)
				for k, v := range r.Symbols() {
					fmt.Printf("%v %v %v\n", m.vcdTime, k, v)
					m.vcd.SetValue(now, v, k)
				}
				m.vcdTime += 10
			}
		}
	}
}
