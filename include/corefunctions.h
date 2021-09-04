#pragma once

#include <Arduino.h>

#include "corefunctionsconfig.h"
#include "logger.h"

namespace arduino_temp {

// Provides ESPcore functionalities.
class CoreFunctions {
 private:
  // CoreFunctions configuration
  CoreFunctionsConfig coreFunctionsConfig_;

  // logging
  Logger logger_;

 public:
  // Constructor
  // parameters:
  //   coreFunctionsConfig ...CoreFunctions configuration
  //   logger ... logging
  CoreFunctions(CoreFunctionsConfig coreFunctionsConfig, Logger logger);

  // Get the reset reason.
  // parameters: none
  // returns the reset reason
  String determineResetReason();

  // Calculate the sleep time.
  // parameters:
  //   sleepTimeInMicros ... time to sleep in micro seconds
  //   startTimeInMicros ... start time in micro seconds
  // return the time to sleep in micro seconds
  long calcSleepTimeInMicroSeconds(int sleepTimeInMicros,
                                   int startTimeInMicros);

  // Do the sleeping for the provided micro seconds.
  // parameters:
  //   sleepTimeInMicros ... the sleep time in micro seconds
  // returns nothing
  void doSleepForMicros(long sleepTimeInMicros);
};

}  // namespace arduino_temp