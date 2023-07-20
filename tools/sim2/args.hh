#pragma once

#include <string>

struct Args {
  std::string firmware_path;
  bool        enable_gdb;
};

Args ParseArgs(int argc, char **argv);
