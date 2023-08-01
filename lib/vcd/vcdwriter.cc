#include "vcdwriter.hh"

#include <assert.h>

#include <format>
#include <limits>
#include <sstream>

namespace mouse::vcd {

namespace {
constexpr std::array<char, 64> kChars = {              //
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',  //
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',  //
    'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',  //
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',  //
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',  //
    'Y', 'Z', '!', '@', '#', '$', '%', '^', '&', '*',  //
    '-', '_', '+', '='};

constexpr std::string BuildID(uint32_t id) {
  std::string result;
  result.reserve(6);
  result.push_back(kChars[id & 0x3f]);
  for (id >>= 6; id > 0; id >>= 6) {
    result.push_back(kChars[id & 0x3f]);
  }
  return result;
}

constexpr std::array<std::string, 16384> BuildCache() {
  std::array<std::string, 16384> result;
  for (size_t i = 0; i < result.size(); i++) {
    result[i] = BuildID(i);
  }
  return result;
}

}  // namespace

VCDWriter::VCDWriter()  //
    : out_(),           //
      scope_(),
      ids_(),
      cache_(BuildCache()),
      buffer_(),
      time_(std::numeric_limits<uint64_t>::max()),
      header_closed_(false) {}

void VCDWriter::Open(const std::string& filename) {
  assert(!out_.is_open());
  out_.open(filename);
}

void VCDWriter::Time(uint64_t time) {
  assert(scope_.empty());
  assert(time_ <= time);

  if (!header_closed_) {
    if (ids_.empty()) {
      // first Time() call, start the header.
      out_ << "$version mouse::vcd::VCDWriter $end" << std::endl;
      out_ << "$timescale 1ns $end" << std::endl;
      out_ << "$scope module top $end" << std::endl;
      time_ = time;
      return;
    }

    // Second Time() call, close the header, and flush the buffer.
    out_ << "$upscope $end" << std::endl;
    out_ << "$enddefinitions $end" << std::endl;
    header_closed_ = true;
    out_ << buffer_.str();
    buffer_.clear();
  }

  if (time_ == time) {
    return;
  }

  time_ = time;
  out_ << "#" << time_ << std::endl;
}

void VCDWriter::BeginScope(const std::string& name) {
  std::ostringstream oss;
  for (const auto& s : scope_) {
    oss << s << ".";
  }
  oss << name;
  scope_.push_back(oss.str());
}

void VCDWriter::EndScope() {
  assert(!scope_.empty());
  scope_.pop_back();
}

std::tuple<std::string, std::string> VCDWriter::GetNameAndID(const std::string& name) {
  std::string full_name;
  if (!scope_.empty()) {
    full_name.append(scope_.back());
  }
  full_name.append(name);

  if (ids_.count(full_name) == 0) {
    ids_[full_name] = ids_.size();
  }

  return {full_name, cache_[ids_.at(full_name)]};
}

void VCDWriter::WriteValue(const std::string& name, size_t width, std::string value) {
  auto [full_name, id] = GetNameAndID(name);

  if (!header_closed_) {
    out_ << "$var wire " << width << " " << id << " " << full_name << " $end" << std::endl;
    out_ << value << id << std::endl;
  } else {
    buffer_ << value << id << std::endl;
  }
}

void VCDWriter::Value(const std::string& name, uint8_t value) {
  WriteValue(name, 8, std::format("{}", value));
}

void VCDWriter::Value(const std::string& name, uint16_t value) {
  WriteValue(name, 16, std::format("{}", value));
}

void VCDWriter::Value(const std::string& name, uint32_t value) {
  WriteValue(name, 32, std::format("{}", value));
}

void VCDWriter::Value(const std::string& name, int8_t value) {
  WriteValue(name, 8, std::format("{}", value));
}

void VCDWriter::Value(const std::string& name, int16_t value) {
  WriteValue(name, 16, std::format("{}", value));
}

void VCDWriter::Value(const std::string& name, int32_t value) {
  WriteValue(name, 32, std::format("{}", value));
}

void VCDWriter::Value(const std::string& name, float value) {
  WriteValue(name, 32, std::format("{}", *reinterpret_cast<uint32_t*>(&value)));
}

void VCDWriter::Value(const std::string& name, bool value) {
  WriteValue(name, 1, value ? "1" : "0");
}

}  // namespace mouse::vcd
