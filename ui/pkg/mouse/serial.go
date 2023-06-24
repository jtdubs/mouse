package mouse

import (
	"bytes"
	"context"
	"encoding/base64"
	"encoding/binary"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"

	"go.bug.st/serial"
)

var (
	portName   = flag.String("port", "/dev/ttyNano", "Serial port")
	baudRate   = flag.Int("baud", 115200, "Baud rate")
	scrollback = flag.Int("scrollback", 100000, "Number of lines to keep in the scrollback buffer")
)

type SerialInterface struct {
	status   string
	portOpen bool
	buffer   []byte
	index    int
	messages *Ring[string]
	report   Report
	sendChan chan Command
}

func NewSerialInterface() *SerialInterface {
	return &SerialInterface{
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

func (s *SerialInterface) Run(ctx context.Context) {
	signalChan := make(chan os.Signal, 1)
	signal.Notify(signalChan, syscall.SIGHUP, syscall.SIGCONT)

	defer func() {
		s.status = "Canceled"
		s.portOpen = false
	}()

	for {
		port := s.open(ctx, signalChan)
		if port == nil {
			return
		}
		if !s.read(ctx, port, signalChan) {
			return
		}
	}
}

func (s *SerialInterface) PortName() string {
	return *portName
}

func (s *SerialInterface) Open() bool {
	return s.portOpen
}

func (s *SerialInterface) Status() string {
	return s.status
}

func (s *SerialInterface) ForEachMessage(f func(string)) {
	s.messages.ForEach(f)
}

func (s *SerialInterface) Clear() {
	s.messages.Clear()
}

func (s *SerialInterface) Report() *Report {
	return &s.report
}

func (s *SerialInterface) SendCommand(c Command) {
	if s.portOpen {
		s.sendChan <- c
	}
}

func (s *SerialInterface) open(ctx context.Context, signalChan <-chan os.Signal) serial.Port {
	s.portOpen = false
	s.status = "Polling"

	for {
		port, err := serial.Open(*portName, &serial.Mode{BaudRate: *baudRate})
		if err == nil {
			port.SetReadTimeout(100 * time.Millisecond)
			s.portOpen = true
			s.status = "Open"
			return port
		}

		select {
		case <-ctx.Done():
			return nil

		case sig := <-signalChan:
			if sig == syscall.SIGHUP {
				if !s.sleep(ctx, signalChan) {
					return nil
				}
			}
			s.status = "Polling"

		case <-time.After(1 * time.Second):
			if len(s.status) > 11 {
				s.status = "Polling."
			} else {
				s.status = s.status + "."
			}
		}
	}
}

func (s *SerialInterface) read(ctx context.Context, port serial.Port, signalChan <-chan os.Signal) bool {
	defer port.Close()

	closedChan := make(chan struct{})

	go func() {
		defer close(closedChan)
		for {
			n, err := port.Read(s.buffer[s.index:])
			if err != nil {
				return
			}
			if n == 0 {
				continue
			}
			s.index += n
			if end := bytes.IndexRune(s.buffer[:s.index], '\n'); end >= 0 {
				s.decode(string(s.buffer[:end]))
				copy(s.buffer, s.buffer[end+1:s.index])
				s.index -= end + 1
			}
		}
	}()

	for {
		select {
		case <-ctx.Done():
			port.Close()
			s.portOpen = false
			return false
		case <-signalChan:
			port.Close()
			s.portOpen = false
			if !s.sleep(ctx, signalChan) {
				return false
			}
			return true
		case cmd := <-s.sendChan:
			var b bytes.Buffer
			binary.Write(&b, binary.LittleEndian, cmd)
			c := fmt.Sprintf("[%s]\n", base64.StdEncoding.EncodeToString(b.Bytes()))
			s.messages.Add(fmt.Sprintf("Sending %q", c))
			port.Write([]byte(c))
		case <-closedChan:
			return true
		}
	}
}

func (s *SerialInterface) sleep(ctx context.Context, signalChan <-chan os.Signal) bool {
	s.status = "Paused"
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

func (s *SerialInterface) decode(message string) {
	if !strings.HasPrefix(message, "[") || !strings.HasSuffix(message, "]") {
		s.messages.Add(fmt.Sprintf("Invalid message: %q", message))
		return
	}

	bs, err := base64.StdEncoding.DecodeString(message[1 : len(message)-1])
	if err != nil {
		s.messages.Add(fmt.Sprintf("Error decoding %q: %v", message, err))
		return
	}

	if len(bs) != binary.Size(Report{}) {
		s.messages.Add(fmt.Sprintf("Incorrect size for %q: got %v, want %v", message, len(bs), binary.Size(Report{})))
		return
	}

	if err := binary.Read(bytes.NewReader(bs), binary.LittleEndian, &s.report); err != nil {
		s.messages.Add(fmt.Sprintf("Error reading %q: %v", message, err))
		return
	}
}
