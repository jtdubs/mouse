#pragma once

#include <string>

namespace remote {

class Remote {
 public:
  Remote();
  bool Init(std::string port);
};

}  // namespace remote
