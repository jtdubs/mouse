#pragma once

#include <simavr/sim_avr.h>

namespace app::sim {

class LEDs {
 public:
  LEDs();

  void Init(avr_t *avr);

  bool GetLeft() const;
  bool GetRight() const;
  bool GetOnboard() const;
  bool GetIR() const;

 private:
  void OnIOWrite(avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param);

 private:
  avr_t *avr_;
  bool   right_, left_, ir_, onboard_;
};

}  // namespace app::sim
