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
    : out_(nullptr),    //
      scope_(),
      ids_(),
      cache_(BuildCache()),
      buffer_(),
      time_(0),
      state_(State::Empty) {}

VCDWriter::VCDWriter(std::shared_ptr<std::ostream> out)  //
    : out_(std::move(out)),                              //
      scope_(),
      ids_(),
      cache_(BuildCache()),
      buffer_(),
      time_(0),
      state_(State::Empty) {}

void VCDWriter::Open(const std::string& filename) {
  assert(out_ == nullptr);
  auto file = std::make_shared<std::ofstream>();
  file->open(filename);
  out_ = std::move(file);
}

void VCDWriter::Time(uint64_t time) {
  assert(scope_.empty());
  assert(time_ <= time);

  switch (state_) {
    case State::Empty:
      *out_ << "$version mouse::vcd::VCDWriter $end" << std::endl;
      *out_ << "$timescale 1ns $end" << std::endl;
      *out_ << "$scope module top $end" << std::endl;
      time_ = time;
      buffer_ << "#" << time_ << std::endl;
      state_ = State::Header;
      break;

    case State::Header:
      *out_ << "$upscope $end" << std::endl;
      *out_ << "$enddefinitions $end" << std::endl;
      state_ = State::Data;
      *out_ << buffer_.str();
      buffer_.clear();
      [[fallthrough]];

    case State::Data:
      if (time_ == time) {
        return;
      }
      time_ = time;
      *out_ << "#" << time_ << std::endl;
      break;
  }
}

void VCDWriter::BeginScope(const std::string& name) {
  assert(state_ != State::Empty);

  std::ostringstream oss;
  for (const auto& s : scope_) {
    oss << s << ".";
  }
  oss << name;
  scope_.push_back(oss.str());

  if (state_ == State::Header) {
    *out_ << "$scope module " << name << " $end" << std::endl;
  }
}

void VCDWriter::EndScope() {
  assert(state_ != State::Empty);
  assert(!scope_.empty());

  scope_.pop_back();

  if (state_ == State::Header) {
    *out_ << "$upscope $end" << std::endl;
  }
}

std::tuple<std::string, std::string> VCDWriter::GetNameAndID(const std::string& name) {
  std::string full_name;
  if (!scope_.empty()) {
    full_name.append(scope_.back());
    full_name.append(".");
  }
  full_name.append(name);

  switch (state_) {
    case State::Empty:
      assert(false);
      break;
    case State::Header:
      if (ids_.count(full_name) == 0) {
        ids_[full_name] = ids_.size();
      }
      break;
    case State::Data:
      break;
  }

  assert(ids_.count(full_name) == 1);
  return {full_name, cache_[ids_.at(full_name)]};
}

void VCDWriter::WriteValue(const std::string& name, size_t width, std::string value) {
  assert(state_ != State::Empty);

  auto [full_name, id] = GetNameAndID(name);

  switch (state_) {
    case State::Empty:
      assert(false);
      break;
    case State::Header:
      *out_ << "$var wire " << width << " " << id << " " << full_name;
      if (width > 1) {
        *out_ << "[" << width - 1 << ":0]";
      }
      *out_ << " $end" << std::endl;
      buffer_ << value << id << std::endl;
      break;
    case State::Data:
      *out_ << value << id << std::endl;
      break;
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
  WriteValue(name, 8, std::format("{}", static_cast<uint8_t>(value)));
}

void VCDWriter::Value(const std::string& name, int16_t value) {
  WriteValue(name, 16, std::format("{}", static_cast<uint16_t>(value)));
}

void VCDWriter::Value(const std::string& name, int32_t value) {
  WriteValue(name, 32, std::format("{}", static_cast<uint32_t>(value)));
}

void VCDWriter::Value(const std::string& name, float value) {
  WriteValue(name, 32, std::format("{}", *reinterpret_cast<uint32_t*>(&value)));
}

void VCDWriter::Value(const std::string& name, bool value) {
  WriteValue(name, 1, value ? "1" : "0");
}

}  // namespace mouse::vcd
