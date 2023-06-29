package mouse

import (
	"bytes"
	"context"
	"encoding/base64"
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"strings"
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
	Recording bool
	status    string
	portOpen  bool
	buffer    []byte
	index     int
	messages  *Ring[string]
	report    Report
	sendChan  chan Command
	vcd       vcd.VcdWriter
	vcdTime   uint64
}

func New() *Mouse {
	return &Mouse{
		buffer:   make([]byte, 256),
		index:    0,
		status:   "Closed",
		portOpen: false,
		messages: NewRing[string](*scrollback),
		report: Report{
			BatteryVolts: 0,
			Mode:         0,
		},
		sendChan: make(chan Command, 1),
	}
}

func (m *Mouse) SetRecording(recording bool) {
	if recording == m.Recording {
		return
	}

	if recording {
		var err error
		m.vcd, err = vcd.New("mouse.vcd", "1ms")
		if err != nil {
			log.Printf("Failed to enable recording: %v", err)
			return
		}
		m.vcd.RegisterVariableList("mouse", m.report.Variables())
		m.vcdTime = 0
	} else {
		m.vcd.Close()
	}

	m.Recording = recording
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

func (m *Mouse) Report() *Report {
	return &m.report
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
			n, err := port.Read(m.buffer[m.index:])
			if err != nil {
				return
			}
			if n == 0 {
				continue
			}
			m.index += n
			if end := bytes.IndexRune(m.buffer[:m.index], '\n'); end >= 0 {
				m.decode(string(m.buffer[:end]))
				copy(m.buffer, m.buffer[end+1:m.index])
				m.index -= end + 1
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
			var b bytes.Buffer
			binary.Write(&b, binary.LittleEndian, cmd)
			c := fmt.Sprintf("[%s]\n", base64.StdEncoding.EncodeToString(b.Bytes()))
			m.messages.Add(fmt.Sprintf("Sending %q", c))
			port.Write([]byte(c))
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

func (m *Mouse) decode(message string) {
	if !strings.HasPrefix(message, "[") || !strings.HasSuffix(message, "]") {
		m.messages.Add(fmt.Sprintf("Invalid message: %q", message))
		return
	}

	bs, err := base64.StdEncoding.DecodeString(message[1 : len(message)-1])
	if err != nil {
		m.messages.Add(fmt.Sprintf("Error decoding %q: %v", message, err))
		return
	}

	if len(bs) != binary.Size(Report{}) {
		m.messages.Add(fmt.Sprintf("Incorrect size for %q: got %v, want %v", message, len(bs), binary.Size(Report{})))
		return
	}

	if err := binary.Read(bytes.NewReader(bs), binary.LittleEndian, &m.report); err != nil {
		m.messages.Add(fmt.Sprintf("Error reading %q: %v", message, err))
		return
	}

	if m.Recording {
		for k, v := range m.report.Symbols() {
			m.vcd.SetValue(m.vcdTime, v, k)
		}
		m.vcdTime += 10
	}
}
