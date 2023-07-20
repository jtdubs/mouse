#pragma once

#include <simavr/sim_avr.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace sim {

struct Symbol {
  std::string name;
  uint64_t    address;
  uint64_t    size;
  void       *data;
};

std::vector<Symbol> ReadSymbols(avr_t *avr, std::string firmware_path);

}  // namespace sim
