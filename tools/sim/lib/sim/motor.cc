#include "motor.hh"

#include <simavr/avr_ioport.h>
#include <simavr/avr_timer.h>

namespace mouse::app::sim {

namespace {
constexpr float kLeftMotorM  = 2.760;
constexpr float kLeftMotorB  = -67.2;
constexpr float kRightMotorM = 2.695;
constexpr float kRightMotorB = -57.7;
}  // namespace

Motor::Motor(bool left)  //
    : left_(left),
      clockwise_(!left),
      desired_period_(0),
      actual_period_(0),
      pulse_index_(0),
      pwm_irq_(nullptr),
      clk_irq_(nullptr),
      dir_irq_(nullptr),
      b_irq_(nullptr),
      avr_(nullptr),
      rpm_(0),
      encoder_callback_(nullptr) {}

void Motor::Init(avr_t *avr) {
  avr_ = avr;

  if (left_) {
    pwm_irq_ = avr_io_getirq(avr_, AVR_IOCTL_TIMER_GETIRQ('1'), 0);
    clk_irq_ = avr_io_getirq(avr_, AVR_IOCTL_IOPORT_GETIRQ('D'), 2);
    b_irq_   = avr_io_getirq(avr_, AVR_IOCTL_IOPORT_GETIRQ('D'), 4);
    dir_irq_ = avr_io_getirq(avr_, AVR_IOCTL_IOPORT_GETIRQ('D'), 7);
  } else {
    pwm_irq_ = avr_io_getirq(avr_, AVR_IOCTL_TIMER_GETIRQ('1'), 1);
    clk_irq_ = avr_io_getirq(avr_, AVR_IOCTL_IOPORT_GETIRQ('D'), 3);
    b_irq_   = avr_io_getirq(avr_, AVR_IOCTL_IOPORT_GETIRQ('D'), 5);
    dir_irq_ = avr_io_getirq(avr_, AVR_IOCTL_IOPORT_GETIRQ('B'), 0);
  }

  avr_irq_register_notify(
      pwm_irq_,
      [](avr_irq_t *irq, uint32_t value, void *param) {  //
        static_cast<Motor *>(param)->OnPWMIRQ(irq, value, param);
      },
      this);

  avr_irq_register_notify(
      dir_irq_,
      [](avr_irq_t *irq, uint32_t value, void *param) {  //
        static_cast<Motor *>(param)->OnDirIRQ(irq, value, param);
      },
      this);
}

float Motor::GetRPM() const {
  return rpm_;
}

void Motor::SetEncoderCallback(std::function<void(bool, bool)> callback) {
  encoder_callback_ = callback;
}

void Motor::OnPWMIRQ([[maybe_unused]] avr_irq_t *irq, uint32_t value, [[maybe_unused]] void *param) {
  if ((left_ && value < 35) || (!left_ && value < 30)) {
    // Not enough power to start the motor turning.
    desired_period_ = 0;
    return;
  }

  rpm_ = left_ ? (kLeftMotorM * value + kLeftMotorB)  //
               : (kRightMotorM * value + kRightMotorB);

  desired_period_ = (16000000.0 * 60.0 / 240.0) / rpm_;

  if (actual_period_ == 0) {
    avr_cycle_timer_register(
        avr_,  //
        desired_period_,
        [](avr_t *avr, avr_cycle_count_t when, void *p) -> avr_cycle_count_t {
          return static_cast<Motor *>(p)->OnCycle(avr, when, p);
        },
        this);
  }
}

void Motor::OnDirIRQ([[maybe_unused]] avr_irq_t *irq, uint32_t value, [[maybe_unused]] void *param) {
  clockwise_ = (value != 0);
}

avr_cycle_count_t Motor::OnCycle([[maybe_unused]] avr_t *avr, avr_cycle_count_t when, [[maybe_unused]] void *param) {
  if (actual_period_ > 0) {
    pulse_index_ = (pulse_index_ + (clockwise_ ? 1 : 3)) % 4;
    if (encoder_callback_) {
      encoder_callback_(left_, clockwise_);
    }
  }

  avr_raise_irq(b_irq_, (pulse_index_ >> 1) & 1);
  avr_raise_irq(clk_irq_, pulse_index_ & 1);

  actual_period_ = desired_period_;
  if (actual_period_ == 0) {
    return 0;
  }

  return when + actual_period_;
}

}  // namespace mouse::app::sim
