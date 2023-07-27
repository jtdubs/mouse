#pragma once

#include <string>

struct Args {
  std::string port;
};

Args ParseArgs(int argc, char **argv);
