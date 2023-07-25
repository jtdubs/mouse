#include "battery.hh"

#include <simavr/avr_adc.h>
#include <simavr/sim_vcd_file.h>

namespace sim {

Battery::Battery(int channel)
    : avr_(nullptr), value_irq_(nullptr), read_irq_(nullptr), voltage_(9000), channel_(channel) {}

void Battery::Init(avr_t *avr) {
  avr_       = avr;
  value_irq_ = avr_io_getirq(avr_, AVR_IOCTL_ADC_GETIRQ, channel_);
  read_irq_  = avr_io_getirq(avr_, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_OUT_TRIGGER);

  avr_irq_register_notify(
      read_irq_,
      [](avr_irq_t *irq, uint32_t value, void *param) { static_cast<Battery *>(param)->OnIRQ(irq, value, param); },
      this);

  avr_vcd_add_signal(avr_->vcd, value_irq_, 16, "BATTERY_VOLTAGE");
}

uint32_t Battery::GetVoltage() const {
  return voltage_;
}

void Battery::SetVoltage(uint32_t voltage) {
  voltage_ = voltage;
}

void Battery::OnIRQ(avr_irq_t *irq, uint32_t value, void *param) {
  avr_raise_irq(value_irq_, voltage_ >> 1);
}

}  // namespace sim
