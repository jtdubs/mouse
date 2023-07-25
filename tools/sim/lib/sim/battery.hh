#pragma once

#include <simavr/sim_avr.h>

namespace sim {

class Battery {
 public:
  Battery(int channel);

  void Init(avr_t *avr);

  uint32_t GetVoltage() const;
  void     SetVoltage(uint32_t voltage);

 private:
  void OnIRQ(avr_irq_t *irq, uint32_t value, void *param);

 private:
  avr_t     *avr_;
  avr_irq_t *value_irq_, *read_irq_;
  uint32_t   voltage_;
  int        channel_;
};

}
