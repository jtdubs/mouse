#pragma once

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/remote/command.hh"
#include "firmware/lib/report/report.hh"
#include "firmware/lib/utils/dequeue.hh"
#include "firmware/platform/platform.hh"

namespace mouse::app::remote {

class Remote {
 public:
  Remote();
  ~Remote();

  bool Init(std::string port);

  bool IsConnected() const;

  void Send(const mode::remote::Command &command);

  std::mutex &GetMutex();

  const platform::Report          &GetPlatformReport() const;
  const control::Report           &GetControlReport() const;
  const maze::Maze                &GetMaze() const;
  const std::list<maze::Location> &GetNextDequeue() const;
  const std::list<maze::Location> &GetPathDequeue() const;

 private:
  void SerialLoop();
  int  TryOpen();
  void Read(int port);
  void OnReport(report::Report *report);

 private:
  std::string                       port_;
  std::stop_source                  stop_;
  std::unique_ptr<std::thread>      thread_;
  platform::Report                  platform_report_;
  control::Report                   control_report_;
  maze::Maze                        maze_;
  std::list<maze::Location>         next_dequeue_;
  std::list<maze::Location>         path_dequeue_;
  std::mutex                        mutex_;
  bool                              connected_;
  std::vector<std::vector<uint8_t>> outbox_;
};

}  // namespace mouse::app::remote
