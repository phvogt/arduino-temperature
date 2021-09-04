#pragma once

#include <Arduino.h>

#include "config.h"
#include "corefunctions.h"
#include "filehandler.h"
#include "ftp.h"
#include "logger.h"
#include "measure.h"
#include "mqtt.h"
#include "ntp.h"
#include "timing.h"
#include "wifi.h"

namespace arduino_temp {

// Program.
class Program {
 private:
  // Configuration
  Config config_;

  // NTP client
  NTP ntp_;
  // logging
  Logger logger_;
  // File handling
  FileHandler fileHandler_;
  // timing
  Timing timing_;
  // MQTT communication
  MQTT mqtt_;
  // Wifi client
  Wifi wifi_;
  // Core functions
  CoreFunctions coreFunctions_;
  // measurement of temperature / humidity
  Measure measure_;
  // FTP client
  FTP ftp_;

  // milli seconds on start
  unsigned long startMillis_ = 0;

 public:
  // Constructor.
  // parameters: none
  Program();

  // do the setup
  void setup();

  // do the loop
  void loop();

  // Sets the milli seconds since start
  // parameters:
  //   startMillis ... the milli seconds since start
  // returns nothing
  void setStartMillis(unsigned long startMillis);

  // Do the normal work.
  // parameters: none
  // returns nothing
  void doWorkNormal();

  // Initializes the NTP.
  // parameters: none
  // returns the current time as epoch
  time_t initNTP();

  // Sends the measured values via MQTT.
  // parameters:
  //   resetReason ... the reset reason
  //   measured_values_bat ... the measured value for the battery
  //   measured_values_dht ... the measured value with MQTT
  // returns the current time as epoch
  void sendMQTTValues(const String& resetReason,
                      const struct measured_values_bat& measuredValuesBatt,
                      const struct measured_values_dht& measuredValuesDht);

  // Send the whole duration via MQTT.
  // parameters:
  //   startMillis ... the milliseconds since the start of the program
  // returns the duration
  long sendMQTTDuration(unsigned long startMillis);

  // Sends the sleep time in milliseconds.
  // parameters:
  //   sleepTimeInMicros ... the sleep time in microseconds
  // returns the current time as epoch
  void sendMQTTSleepTime(long sleepTimeInMicros);

  // Send the timings via MQTT.
  // parameters: none
  // returns nothing
  void sendMQTTTimings();

  // Calculates the time to sleep.
  // parameters:
  //   startMillis ... the milliseconds since the start of the program
  // returns the calculated sleep time in micro seconds
  long calculateSleepTimeMicros(unsigned long startMillis);

  // Dump the current log file buffer to the log file.
  // parameters:
  //   logfileName ... base name for log files
  // returns nothing
  void dumpLog(String logfileName);

  // Rotate the log files.
  // parameters:
  //   logfileName ... base name for log files
  // returns nothing
  void rotateLogs(String logfileName);
};

}  // namespace arduino_temp