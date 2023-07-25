#pragma once

#include <simavr/sim_avr.h>
#include <simavr/sim_elf.h>
#include <simavr/sim_gdb.h>

extern "C" {
#include <simavr/parts/uart_pty.h>
}

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <semaphore>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#include "irbeam.hh"
#include "maze.hh"
#include "mouse.hh"
#include "position.hh"
#include "symbols.hh"

namespace sim {

enum class State {
  Paused,
  Running,
  Crashed,
  Done,
};

typedef std::function<bool()> BreakpointFn;

class Sim {
 public:
  Sim();
  ~Sim();

  bool     Init(std::string firmware_path, bool enable_gdb);
  State    GetState() const;
  uint64_t GetMicros() const;
  uint64_t GetNanos() const;

  bool IsRecording() const;
  void SetRecording(bool recording);

  void Run();
  void Pause();
  void RunNearest(uint64_t targetNanos);
  void RunUntil(BreakpointFn fn);

  Mouse& GetMouse();

  std::shared_ptr<Maze> GetMaze() const;
  void                  SetMaze(std::string name);

  const std::map<std::string, Symbol>& GetSymbols() const;

  const Position& GetMousePos() const;
  void            SetMousePos(Position pos);

  float GetMouseTheta() const;
  void  SetMouseTheta(float theta);

  Position Project(Position mousePosition) const;

  std::vector<IRBeam> GetIRBeams();

 private:
  void RunLoop();

  IRBeam GetIRBeam(Sensor& sensor);

  void OnEncoderClock(bool left, bool clockwise);
  void OnSensorRead();

 private:
  avr_t*                        avr_;
  uart_pty_t                    pty_;
  elf_firmware_t                firmware_;
  State                         state_;
  std::unique_ptr<std::thread>  thread_;
  std::stop_source              stop_;
  BreakpointFn                  breakpoint_;
  bool                          recording_;
  Mouse                         mouse_;
  std::map<std::string, Symbol> symbols_;
  std::shared_ptr<Maze>         maze_;
  Position                      mouse_pos_;
  float                         mouse_theta_;
  std::vector<IRBeam>           ir_beams_;
  std::mutex                    ir_mutex_;
};

}  // namespace sim
