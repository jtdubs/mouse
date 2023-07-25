#include "leds.hh"

namespace sim {

LEDs::LEDs() : avr_(nullptr), right_(false), left_(false), ir_(false), onboard_(false) {}

void LEDs::Init(avr_t *avr) {
  avr_ = avr;
  avr_register_io_write(
      avr_, 0x25,
      [](avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
        static_cast<LEDs *>(param)->OnIOWrite(avr, addr, v, param);
      },
      this);
  avr_register_io_write(
      avr_, 0x2B,
      [](avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
        static_cast<LEDs *>(param)->OnIOWrite(avr, addr, v, param);
      },
      this);
}

void LEDs::OnIOWrite(avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
  switch (addr) {
    case 0x25:
      left_    = ((v >> 3) & 1) == 1;
      ir_      = ((v >> 4) & 1) == 1;
      onboard_ = ((v >> 5) & 1) == 1;
      break;
    case 0x2B:
      right_ = ((v >> 6) & 1) == 1;
      break;
    default:
      break;
  }
}

bool LEDs::GetLeft() const {
  return left_;
}

bool LEDs::GetRight() const {
  return right_;
}

bool LEDs::GetOnboard() const {
  return onboard_;
}

bool LEDs::GetIR() const {
  return ir_;
}

}  // namespace sim
