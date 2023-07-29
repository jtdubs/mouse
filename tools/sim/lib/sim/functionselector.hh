#pragma once

#include <simavr/sim_avr.h>

namespace mouse::app::sim {

class FunctionSelector {
 public:
  FunctionSelector(int channel);

  void Init(avr_t *avr);

  uint8_t GetFunction() const;
  void    SetFunction(uint8_t voltage);

  bool GetButton() const;
  void SetButton(bool voltage);

 private:
  void UpdateVoltage();
  void OnIRQ(avr_irq_t *irq, uint32_t value, void *param);

 private:
  avr_t     *avr_;
  int        channel_;
  avr_irq_t *value_irq_, *read_irq_;
  uint32_t   voltage_;
  uint8_t    function_;
  bool       button_;
};

}  // namespace mouse::app::sim
