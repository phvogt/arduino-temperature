#pragma once

#include <Arduino.h>

namespace arduino_temp {

// Core function confguration.
class CoreFunctionsConfig {
 public:
  // factor to apply to calculated sleep time
  float deepSleepFactor;
};

}  // namespace arduino_temp