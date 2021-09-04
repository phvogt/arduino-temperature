#pragma once

#include <Arduino.h>

#include "corefunctionsconfig.h"

namespace arduino_temp {

// Provides ESPcore functionalities.
class CoreFunctions {
 private:
  // CoreFunctions configuration
  CoreFunctionsConfig coreFunctionsConfig_;

 public:
  // Constructor
  // parameters:
  //   coreFunctionsConfig ...CoreFunctions configuration
  CoreFunctions(CoreFunctionsConfig coreFunctionsConfig);

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
};

}  // namespace arduino_temp