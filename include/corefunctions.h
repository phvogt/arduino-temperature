#pragma once

#include <Arduino.h>

#include "filehandler.h"

namespace arduino_temp {

// Provides ESPcore functionalities.
class CoreFunctions {
 private:
  // file handler
  FileHandler filehandler_;

 public:
  // Get the reset reason.
  // parameters: none
  // returns the reset reason
  String determineResetReason();

  // Calculate the sleep time.
  // parameters:
  //   sleepTimeInMillis ... time to sleep in milli seconds
  //   startTimeInMillis ... start time in milli seconds
  // return the time to sleep in micro seconds
  long calcSleepTimeInMicroSeconds(int sleepTimeInMillis,
                                   int startTimeInMillis);

  // Do the sleeping for the provided micro seconds.
  // parameters:
  //   sleepTimeInMicros ... the sleep time in micro seconds
  // returns nothing
  void doSleepForMicros(long sleepTimeInMicros);

  // Sleeps fot the maximum time.
  // parameters: none
  // returns nothing
  void sleepMaxTime();
};

}  // namespace arduino_temp