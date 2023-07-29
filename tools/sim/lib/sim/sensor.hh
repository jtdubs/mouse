#pragma once

#include <simavr/sim_avr.h>

#include <functional>
#include <string>

#include "position.hh"

namespace mouse::app::sim {

class Sensor {
 public:
  Sensor(std::string name, int channel, Position pos, float theta);

  void Init(avr_t *avr);

  void SetReadCallback(std::function<void()> callback);

  uint32_t GetVoltage() const;
  void     SetVoltage(uint32_t voltage);

  const Position &GetPosition() const;
  float           GetTheta() const;

 private:
  void OnIRQ(avr_irq_t *irq, uint32_t value, void *param);

 private:
  avr_t                *avr_;
  avr_irq_t            *value_irq_, *read_irq_;
  uint32_t              voltage_;
  int                   channel_;
  Position              position_;
  float                 theta_;
  std::function<void()> read_callback_;
  std::string           name_;
};

}  // namespace mouse::app::sim
