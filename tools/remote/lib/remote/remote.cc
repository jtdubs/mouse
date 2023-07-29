#include "remote.hh"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <format>
#include <iostream>

#include "firmware/platform/usart0.hh"

namespace remote {

Remote::Remote()  //
    : port_(),
      stop_(),
      thread_(nullptr),
      platform_report_(),
      control_report_(),
      maze_(),
      next_dequeue_(),
      path_dequeue_(),
      mutex_(),
      connected_(false),
      outbox_() {
  for (size_t x = 0; x < 16; x++) {
    for (size_t y = 0; y < 16; y++) {
      maze::Location loc(x, y);
      if (x == 0) {
        maze_.cells[loc].wall_west = true;
      }
      if (x == 15) {
        maze_.cells[loc].wall_east = true;
      }
      if (y == 0) {
        maze_.cells[loc].wall_south = true;
      }
      if (y == 15) {
        maze_.cells[loc].wall_north = true;
      }
    }
  }
}

Remote::~Remote() {
  stop_.request_stop();
  if (thread_ != nullptr) {
    thread_->join();
  }
}

bool Remote::Init(std::string port) {
  port_   = port;
  thread_ = std::make_unique<std::thread>(&Remote::SerialLoop, this);
  return true;
}

bool Remote::IsConnected() const {
  return connected_;
}

void Remote::Send(const remote::command::Command &command) {
  uint8_t len = sizeof(remote::command::Type);
  switch (command.type) {
    case remote::command::Type::Reset:
    case remote::command::Type::Explore:
    case remote::command::Type::Solve:
    case remote::command::Type::SendMaze:
    case remote::command::Type::PlanExecute:
      break;
    case remote::command::Type::TunePID:
      len += sizeof(remote::command::PidID) + 4 * sizeof(float);
      break;
    case remote::command::Type::PlanEnqueue:
      len += sizeof(plan::Plan);
      break;
  }

  uint8_t chk = 0;
  auto    c   = reinterpret_cast<const uint8_t *>(&command);

  std::vector<uint8_t> buf;
  buf.push_back(usart0::kStartByte);
  buf.push_back(len);
  for (size_t i = 0; i < len; i++) {
    buf.push_back(c[i]);
    chk += c[i];
  }
  buf.push_back(-chk);
  outbox_.push_back(buf);
}

std::mutex &Remote::GetMutex() {
  return mutex_;
}

const platform::Report &Remote::GetPlatformReport() const {
  return platform_report_;
}

const control::Report &Remote::GetControlReport() const {
  return control_report_;
}

const maze::Maze &Remote::GetMaze() const {
  return maze_;
}

const std::list<maze::Location> &Remote::GetNextDequeue() const {
  return next_dequeue_;
}

const std::list<maze::Location> &Remote::GetPathDequeue() const {
  return path_dequeue_;
}

void Remote::SerialLoop() {
  while (!stop_.stop_requested()) {
    int fd = -1;
    while (!stop_.stop_requested()) {
      fd = TryOpen();
      if (fd >= 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (stop_.stop_requested()) {
      return;
    }

    connected_ = true;
    Read(fd);
    connected_ = false;
  }
}

int Remote::TryOpen() {
  // Open the serial port and set to 115200 8-N-1.
  int port = open(port_.c_str(), O_RDWR);
  if (port < 0) {
    std::cerr << std::format("Failed to open serial port {}: {}", port_, strerror(errno)) << std::endl;
    return -1;
  }

  termios tty = {};
  if (tcgetattr(port, &tty) < 0) {
    std::cerr << std::format("Failed to get serial port attributes: {}", strerror(errno)) << std::endl;
    return -1;
  }

  tty.c_cflag &= ~PARENB;         // no parity
  tty.c_cflag &= ~CSTOPB;         // 1 stop bit
  tty.c_cflag &= ~CSIZE;          // clear size bits
  tty.c_cflag |= CS8;             // 8 data bits
  tty.c_cflag &= ~CRTSCTS;        // no hardware flow control
  tty.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                       // no software flow control
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);  // raw input

  tty.c_oflag &= ~OPOST;  // raw output
  tty.c_oflag &= ~ONLCR;  // don't convert linefeeds

  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);  // raw input

  tty.c_cc[VMIN]  = 0;  // read doesn't block
  tty.c_cc[VTIME] = 0;  // read doesn't block

  cfsetspeed(&tty, B115200);  // set baud rate

  if (tcsetattr(port, TCSANOW, &tty) != 0) {
    std::cerr << std::format("Failed to set serial port attributes: {}", strerror(errno)) << std::endl;
    return -1;
  }

  return port;
}

void Remote::Read(int port) {
  usart0::ReadState state = usart0::ReadState::Idle;

  uint8_t report[128];
  ssize_t report_idx = 0;
  ssize_t report_len = 0;
  uint8_t report_chk = 0;
  char    buf[128];

  while (!stop_.stop_requested()) {
    ssize_t n = read(port, buf, sizeof(buf));
    if (n < 0) {
      std::cerr << std::format("Failed to read from serial port: {}", strerror(errno)) << std::endl;
      return;
    }
    if (n == 0) {
      for (auto out : outbox_) {
        ssize_t nwritten = write(port, out.data(), out.size());
        if (nwritten < 0) {
          std::cerr << std::format("Failed to write to serial port: {}", strerror(errno)) << std::endl;
          return;
        }
      }
      outbox_.clear();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    if (n > 0) {
      for (ssize_t idx = 0; idx < n; idx++) {
        switch (state) {
          case usart0::ReadState::Idle:
            assert(idx < n);
            if (buf[idx] == usart0::kStartByte) {
              state = usart0::ReadState::Length;
              continue;
            }
            break;
          case usart0::ReadState::Length:
            assert(idx < n);
            report_len = buf[idx];
            report_idx = 0;
            report_chk = 0;
            if (report_len == 0) {
              state = usart0::ReadState::Idle;
            } else {
              state = usart0::ReadState::Data;
            }
            break;
          case usart0::ReadState::Data: {
            assert(idx < n);
            auto b      = buf[idx];
            report_chk += b;
            assert(report_idx < (ssize_t)sizeof(report));
            report[report_idx++] = b;
            if (report_idx == report_len) {
              state = usart0::ReadState::Checksum;
            }
            break;
          }
          case usart0::ReadState::Checksum:
            state = usart0::ReadState::Idle;
            assert(idx < n);
            report_chk += buf[idx];
            if (report_chk != 0) {
              std::cerr << "Checksum error" << std::endl;
              break;
            }
            OnReport(reinterpret_cast<report::Report *>(report));
            break;
        }
      }
    }
  }
}

void Remote::OnReport(report::Report *report) {
  auto guard = std::lock_guard<std::mutex>(mutex_);

  switch (report->type) {
    case report::Type::Platform:
      platform_report_ = *reinterpret_cast<platform::Report *>(report->data);
      break;
    case report::Type::Control:
      control_report_ = *reinterpret_cast<control::Report *>(report->data);
      break;
    case report::Type::Maze: {
      auto   updates = reinterpret_cast<maze::Update *>(report->data);
      size_t n       = report->length / sizeof(maze::Update);
      for (size_t i = 0; i < n; i++) {
        maze_.cells[updates[i].location] = updates[i].cell;
      }
      break;
    }
    case report::Type::Explore: {
      auto   updates = reinterpret_cast<explore::DequeueUpdate *>(report->data);
      size_t n       = report->length / sizeof(explore::DequeueUpdate);
      for (size_t i = 0; i < n; i++) {
        auto update = updates[i];
        switch (update.dequeue_id) {
          case explore::DequeueID::Next:
            switch (update.event) {
              case dequeue::Event::PushFront:
                next_dequeue_.push_front(update.value);
                break;
              case dequeue::Event::PushBack:
                next_dequeue_.push_back(update.value);
                break;
              case dequeue::Event::PopFront:
                next_dequeue_.pop_front();
                break;
              case dequeue::Event::PopBack:
                next_dequeue_.pop_back();
                break;
            }
            break;
          case explore::DequeueID::Path:
            switch (update.event) {
              case dequeue::Event::PushFront:
                path_dequeue_.push_front(update.value);
                break;
              case dequeue::Event::PushBack:
                path_dequeue_.push_back(update.value);
                break;
              case dequeue::Event::PopFront:
                path_dequeue_.pop_front();
                break;
              case dequeue::Event::PopBack:
                path_dequeue_.pop_back();
                break;
            }
          case explore::DequeueID::Invalid:
            break;
        }
      }
    } break;
  }
}

}  // namespace remote
