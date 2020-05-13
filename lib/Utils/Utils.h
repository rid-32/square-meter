#include <Arduino.h>

#ifndef UTILS
#define UTILS

namespace utils {
uint16_t pow(uint16_t num, int8_t deg) {
  uint16_t acc = 1;

  for (uint8_t i = 1; i <= deg; i++) {
    acc *= num;
  }

  return acc;
}
} // namespace utils

#endif
