#pragma once

#include <simavr/sim_avr.h>

#include <functional>

namespace sim {

class Motor {
 public:
  Motor(bool left);

  void Init(avr_t *avr);

  float GetRPM() const;

  void SetEncoderCallback(std::function<void(bool, bool)> callback);

 private:
  void              OnPWMIRQ(avr_irq_t *irq, uint32_t value, void *param);
  void              OnDirIRQ(avr_irq_t *irq, uint32_t value, void *param);
  avr_cycle_count_t OnCycle(avr_t *avr, avr_cycle_count_t when, void *param);

 private:
  bool                            left_;
  bool                            clockwise_;
  uint32_t                        desired_period_;
  uint32_t                        actual_period_;
  int                             pulse_index_;
  avr_irq_t                      *pwm_irq_;
  avr_irq_t                      *clk_irq_;
  avr_irq_t                      *dir_irq_;
  avr_irq_t                      *b_irq_;
  avr_t                          *avr_;
  float                           rpm_;
  std::function<void(bool, bool)> encoder_callback_;
};

}  // namespace sim
