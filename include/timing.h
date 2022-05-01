#pragma once

#include <Arduino.h>

#include "logger.h"

namespace arduino_temp {

// Timing of events.
class Timing {
 private:
  // logging
  Logger* logger_;

  // for measuring timing
  unsigned long timingStart_ = 0;

  // contains all the timings
  String timings_ = "";

 public:
  // Constructor.
  // parameters:
  //   logger ... logging
  Timing(Logger& logger);

  // Starts the timing measuring.
  // parameters: none
  // returns nothing
  void timingStart();

  // Stops the timing measuring and logs the duration.
  // parameters:
  //   measuredName ... name of the duration that was measured
  // returns the difference between timingStart() and timingEnd() in
  // milliseconds.
  unsigned long timingEnd(String measuredName);

  // Gets the timings so far.
  // parameters: none
  // returns the timings
  String getTimings();
};

}  // namespace arduino_temp