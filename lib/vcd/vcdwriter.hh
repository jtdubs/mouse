#pragma once

#include <stdint.h>

#include <array>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace mouse::vcd {

enum class State { Empty, Header, Data };

class VCDWriter {
 public:
  VCDWriter();
  VCDWriter(std::shared_ptr<std::ostream> out);

  void Open(const std::string& filename);

  void Time(uint64_t time);

  void BeginScope(const std::string& name);
  void EndScope();

  void Value(const std::string& name, uint8_t value);
  void Value(const std::string& name, uint16_t value);
  void Value(const std::string& name, uint32_t value);
  void Value(const std::string& name, int8_t value);
  void Value(const std::string& name, int16_t value);
  void Value(const std::string& name, int32_t value);
  void Value(const std::string& name, float value);
  void Value(const std::string& name, bool value);

 private:
  std::tuple<std::string, std::string> GetNameAndID(const std::string& name);
  void                                 WriteValue(const std::string& name, size_t width, std::string value);

 private:
  std::shared_ptr<std::ostream>   out_;
  std::vector<std::string>        scope_;
  std::map<std::string, uint16_t> ids_;
  std::array<std::string, 16384>  cache_;
  std::ostringstream              buffer_;
  uint64_t                        time_;
  State                           state_;
};

}  // namespace mouse::vcd
