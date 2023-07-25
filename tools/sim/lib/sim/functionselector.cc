#include "functionselector.hh"

#include <simavr/avr_adc.h>
#include <simavr/sim_vcd_file.h>

namespace sim {

constexpr float kFunctionVoltages[16] = {
    3235, 3176, 3098, 3019, 2882, 2784, 2666, 2549, 2274, 2117, 1901, 1686, 1333, 1039, 627, 215,
};

FunctionSelector::FunctionSelector(int channel)
    : avr_(nullptr),
      channel_(channel),
      value_irq_(nullptr),
      read_irq_(nullptr),
      voltage_(9000),
      function_(0),
      button_(false) {}

void FunctionSelector::Init(avr_t *avr) {
  avr_       = avr;
  value_irq_ = avr_io_getirq(avr_, AVR_IOCTL_ADC_GETIRQ, channel_);
  read_irq_  = avr_io_getirq(avr_, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_OUT_TRIGGER);

  avr_irq_register_notify(
      read_irq_,
      [](avr_irq_t *irq, uint32_t value, void *param) {
        static_cast<FunctionSelector *>(param)->OnIRQ(irq, value, param);
      },
      this);

  avr_vcd_add_signal(avr_->vcd, value_irq_, 16, "FUNCTION_SELECT");
}

uint8_t FunctionSelector::GetFunction() const {
  return function_;
}

void FunctionSelector::SetFunction(uint8_t function) {
  function_ = function;
  UpdateVoltage();
}

bool FunctionSelector::GetButton() const {
  return button_;
}

void FunctionSelector::SetButton(bool button) {
  button_ = button;
  UpdateVoltage();
}

void FunctionSelector::UpdateVoltage() {
  if (button_) {
    voltage_ = 5000;
  } else {
    voltage_ = kFunctionVoltages[function_];
  }
}

void FunctionSelector::OnIRQ(avr_irq_t *irq, uint32_t value, void *param) {
  avr_raise_irq(value_irq_, voltage_);
}

}  // namespace sim
