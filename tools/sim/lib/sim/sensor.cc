#include "sensor.hh"

#include <simavr/avr_adc.h>
#include <simavr/sim_vcd_file.h>

namespace app::sim {

Sensor::Sensor(std::string name, int channel, Position pos, float theta)
    : avr_(nullptr),  //
      value_irq_(nullptr),
      read_irq_(nullptr),
      voltage_(0),
      channel_(channel),
      position_(pos),
      theta_(theta),
      name_(name) {}

void Sensor::Init(avr_t *avr) {
  avr_       = avr;
  value_irq_ = avr_io_getirq(avr_, AVR_IOCTL_ADC_GETIRQ, channel_);
  read_irq_  = avr_io_getirq(avr_, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_OUT_TRIGGER);

  avr_irq_register_notify(
      read_irq_,
      [](avr_irq_t *irq, uint32_t value, void *param) { static_cast<Sensor *>(param)->OnIRQ(irq, value, param); },
      this);

  avr_vcd_add_signal(avr_->vcd, value_irq_, 16, name_.c_str());
}

void Sensor::SetReadCallback(std::function<void()> callback) {
  read_callback_ = callback;
}

uint32_t Sensor::GetVoltage() const {
  return voltage_;
}

void Sensor::SetVoltage(uint32_t voltage) {
  voltage_ = voltage;
}

const Position &Sensor::GetPosition() const {
  return position_;
}

float Sensor::GetTheta() const {
  return theta_;
}

void Sensor::OnIRQ([[maybe_unused]] avr_irq_t *irq, [[maybe_unused]] uint32_t value, [[maybe_unused]] void *param) {
  if (read_callback_) {
    read_callback_();
  }
  avr_raise_irq(value_irq_, voltage_);
}

}  // namespace app::sim
