#include "sim.hh"

#include <assert.h>
#include <simavr/avr_adc.h>
#include <simavr/avr_ioport.h>
#include <simavr/avr_timer.h>
#include <simavr/sim_avr.h>
#include <simavr/sim_io.h>
#include <simavr/sim_vcd_file.h>

#include <cmath>
#include <numbers>

#include "symbols.hh"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace sim {

namespace {
constexpr float kGridSize                = 180.0;  // mm
constexpr float kAxleOffset              = 36.0;   // mm
constexpr float kWheelBase               = 72.48;  // mm
constexpr float kWheelDiameter           = 32.5;   // mm
constexpr int   kEncoderTicksPerRotation = 240;    // dimensionless

constexpr float kWheelCircumference     = std::numbers::pi * kWheelDiameter;                                   // mm
constexpr float kDistancePerEncoderTick = kWheelCircumference / static_cast<float>(kEncoderTicksPerRotation);  // mm
constexpr float kEncoderTickDTheta      = kDistancePerEncoderTick / kWheelBase;                       // radians
constexpr float kEncoderTickDx          = (kWheelBase / 2.0) * std::sin(kEncoderTickDTheta);          // mm
constexpr float kEncoderTickDy          = (kWheelBase / 2.0) * (1.0 - std::cos(kEncoderTickDTheta));  // mm
}  // namespace

Sim::Sim()
    : avr_{nullptr},  //
      pty_{0},
      firmware_{0},
      state_(State::Paused),
      thread_(),
      stop_(),
      breakpoint_(nullptr),
      recording_(false),
      mouse_(),
      symbols_(),
      maze_(),
      mouse_pos_(90, 36),
      mouse_theta_(std::numbers::pi / 2.0f),
      ir_beams_(),
      ir_mutex_() {}

Sim::~Sim() {
  Pause();
}

bool Sim::Init(std::string firmware_path, bool enable_gdb) {
  int rc = 0;

  if ((rc = elf_read_firmware(firmware_path.c_str(), &firmware_)) != 0) {
    fprintf(stderr, "elf_read_firmware failed: %d\n", rc);
    return false;
  }

  avr_ = avr_make_mcu_by_name(firmware_.mmcu);
  if (avr_ == nullptr) {
    fprintf(stderr, "avr_make_mcu_by_name failed\n");
    return false;
  }

  if ((rc = avr_init(avr_)) != 0) {
    fprintf(stderr, "avr_init failed: %d\n", rc);
    return false;
  }

  avr_load_firmware(avr_, &firmware_);
  avr_->vcd->filename = const_cast<char*>("sim.vcd");

  if (enable_gdb) {
    avr_->gdb_port = 1234;
    avr_->state    = cpu_Stopped;
    if ((rc = avr_gdb_init(avr_)) != 0) {
      fprintf(stderr, "avr_gdb_init failed: %d\n", rc);
      return false;
    }
  }

  uart_pty_init(avr_, &pty_);
  uart_pty_connect(&pty_, '0');

  auto symbols = ReadSymbols(avr_, firmware_path);
  for (auto symbol : symbols) {
    symbols_[symbol.name] = symbol;
  }

  mouse_.Init(avr_);
  maze_ = Maze::Load(Maze::List().front());

  mouse_.GetLeftSensor().SetReadCallback([&]() { OnSensorRead(); });
  mouse_.GetForwardSensor().SetReadCallback([&]() { OnSensorRead(); });
  mouse_.GetRightSensor().SetReadCallback([&]() { OnSensorRead(); });
  mouse_.GetLeftMotor().SetEncoderCallback([&](bool left, bool clockwise) {  //
    OnEncoderClock(left, clockwise);
  });
  mouse_.GetRightMotor().SetEncoderCallback([&](bool left, bool clockwise) {  //
    OnEncoderClock(left, clockwise);
  });

  return true;
}

State Sim::GetState() const {
  return state_;
}

uint64_t Sim::GetMicros() const {
  return avr_->cycle / 16;
}

uint64_t Sim::GetNanos() const {
  return (avr_->cycle * 62) + (avr_->cycle / 2);
}

bool Sim::IsRecording() const {
  return recording_;
}

void Sim::SetRecording(bool recording) {
  auto state = state_;
  if (state == State::Running) {
    Pause();
  }

  if (recording && !recording_) {
    avr_vcd_start(avr_->vcd);
  } else if (!recording && recording_) {
    avr_vcd_stop(avr_->vcd);
  }
  recording_ = recording;

  if (state == State::Running) {
    Run();
  }
}

void Sim::Run() {
  if (thread_.get() != nullptr) {
    Pause();
  }
  stop_   = std::stop_source();
  thread_ = std::make_unique<std::thread>(&Sim::RunLoop, this);
}

void Sim::Pause() {
  if (thread_.get() == nullptr) {
    return;
  }
  stop_.request_stop();
  thread_->join();
  thread_.reset(nullptr);
}

void Sim::RunNearest(uint64_t targetNanos) {
  if (state_ != State::Paused) {
    return;
  }

  uint64_t now    = GetNanos();
  uint64_t target = now - (now % targetNanos) + targetNanos;
  RunUntil([this, target]() -> bool { return GetNanos() >= target; });
}

void Sim::RunUntil(BreakpointFn fn) {
  breakpoint_ = fn;
  Run();
}

void Sim::RunLoop() {
  if (state_ != State::Paused) {
    return;
  }

  state_ = State::Running;

  while (!stop_.stop_requested()) {
    if (breakpoint_ != nullptr && breakpoint_()) {
      breakpoint_ = nullptr;
      break;
    }

    switch (avr_run(avr_)) {
      case cpu_Done:
        state_ = State::Done;
        break;
      case cpu_Crashed:
        state_ = State::Crashed;
        break;
      default:
        break;
    }
  }

  state_ = State::Paused;
}

Mouse& Sim::GetMouse() {
  return mouse_;
}

std::shared_ptr<Maze> Sim::GetMaze() const {
  return maze_;
}

void Sim::SetMaze(std::string name) {
  maze_ = std::move(Maze::Load(name));
}

const std::map<std::string, Symbol>& Sim::GetSymbols() const {
  return symbols_;
}

const Position& Sim::GetMousePos() const {
  return mouse_pos_;
}

void Sim::SetMousePos(Position pos) {
  mouse_pos_ = pos;
  ir_beams_.clear();
}

float Sim::GetMouseTheta() const {
  return mouse_theta_;
}

void Sim::SetMouseTheta(float theta) {
  mouse_theta_ = theta;
  ir_beams_.clear();
}

Position Sim::Project(Position mousePosition) const {
  auto [x, y] = mousePosition;

  return Position(  //
      mouse_pos_.x + (x * std::cos(mouse_theta_) - y * std::sin(mouse_theta_)),
      mouse_pos_.y + (x * std::sin(mouse_theta_) + y * std::cos(mouse_theta_)));
}

std::vector<IRBeam> Sim::GetIRBeams() {
  std::lock_guard<std::mutex> lock(ir_mutex_);

  if (!mouse_.GetLEDs().GetIR()) {
    ir_beams_.clear();
    mouse_.GetLeftSensor().SetVoltage(0);
    mouse_.GetForwardSensor().SetVoltage(0);
    mouse_.GetRightSensor().SetVoltage(0);
    return ir_beams_;
  }

  if (ir_beams_.size() > 0) {
    return ir_beams_;
  }

  auto ir_beams = std::vector<IRBeam>();
  if (auto beam = GetIRBeam(mouse_.GetLeftSensor()); beam.distance != INFINITY) {
    ir_beams.push_back(beam);
  }
  if (auto beam = GetIRBeam(mouse_.GetForwardSensor()); beam.distance != INFINITY) {
    ir_beams.push_back(beam);
  }
  if (auto beam = GetIRBeam(mouse_.GetRightSensor()); beam.distance != INFINITY) {
    ir_beams.push_back(beam);
  }
  ir_beams_ = ir_beams;
  return ir_beams_;
}

IRBeam Sim::GetIRBeam(Sensor& sensor) {
  int width, height;
  maze_->GetSize(width, height);

  auto best_beam = IRBeam(Position(0, 0), Position(0, 0), INFINITY, 0);
  auto origin    = Project(sensor.GetPosition());
  auto theta     = mouse_theta_ + sensor.GetTheta();

  auto tan_theta = tanf(theta);
  auto sin_sign  = signbit(std::sin(theta));
  auto cos_sign  = signbit(std::cos(theta));

  for (int y = 0; y < height; y++) {
    auto wall_y = y * 180.0;
    auto dy     = wall_y - origin.y;
    auto dx     = 0.0f;
    if (tan_theta != 0.0) {
      dx = dy / tan_theta;
    }
    if (signbit(dx) != cos_sign) {
      continue;
    }
    auto x = origin.x + dx;
    if (x < 0 || x > (width * 180.0)) {
      continue;
    }
    if ((*maze_)(x / 180, y).east) {
      auto dist = std::sqrt(dx * dx + dy * dy);
      if (dist < best_beam.distance) {
        best_beam = IRBeam(origin, Position(x, wall_y), dist, 0);
      }
    }
  }

  for (int x = 0; x < width; x++) {
    auto wall_x = x * 180.0;
    auto dx     = wall_x - origin.x;
    auto dy     = dx * tan_theta;
    if (signbit(dy) != sin_sign) {
      continue;
    }
    auto y = origin.y + dy;
    if (y < 0 || y > (height * 180.0)) {
      continue;
    }
    if ((*maze_)(x, y / 180).north) {
      auto dist = std::sqrt(dx * dx + dy * dy);
      if (dist < best_beam.distance) {
        best_beam = IRBeam(origin, Position(wall_x, y), dist, std::numbers::pi / 2.0f);
      }
    }
  }

  if (best_beam.distance == INFINITY) {
    sensor.SetVoltage(0);
  } else {
    auto scale   = std::abs(std::sin(best_beam.wall_angle - theta));
    auto voltage = scale * std::min(5000.0, 1200000.0 / powf(best_beam.distance, 1000.0 / 583.0));
    sensor.SetVoltage(voltage);
  }

  return best_beam;
}

void Sim::OnSensorRead() {
  (void)GetIRBeams();
}

void Sim::OnEncoderClock(bool left, bool clockwise) {
  bool forward = (left != clockwise);

  float dx = forward ? kEncoderTickDx : -kEncoderTickDx;
  float dy = left ? -kEncoderTickDy : kEncoderTickDy;
  float dt = (left ^ forward) ? kEncoderTickDTheta : -kEncoderTickDTheta;

  auto delta = ImVec2(                                            //
      dx * std::cos(mouse_theta_) - dy * std::sin(mouse_theta_),  //
      dx * std::sin(mouse_theta_) + dy * std::cos(mouse_theta_));

  SetMousePos(static_cast<ImVec2>(GetMousePos()) + delta);
  SetMouseTheta(std::fmod(GetMouseTheta() + dt, 2.0f * std::numbers::pi));
}

}  // namespace sim
