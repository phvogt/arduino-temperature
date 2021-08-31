#pragma once

#include <Arduino.h>

#include "corefunctions.h"
#include "filehandler.h"
#include "ftp.h"
#include "measure.h"
#include "mqtt.h"
#include "timing.h"
#include "wifi.h"

namespace arduino_temp {

// Program
class Program {
 private:
  // file handler
  FileHandler filehandler_;  
  // timing
  Timing timing_;
  
  // MQTT communication
  MQTT mqtt_;
  // Wifi client
  Wifi wifi_;
  // NTP client
  NTP ntp_;
  // Core functions
  CoreFunctions coreFunctions_;
  // measurement of temperature / humidity
  Measure measure_;
  // FTP client
  FTP ftp_;

  // milli seconds on start
  unsigned long startMillis_ = 0;

 public:
  Program();

  // do the setup
  void setup();

  // do the loop
  void loop();

  // Sets the milli seconds since start
  // parameters:
  //   startMillis ... the milli seconds since start
  void setStartMillis(unsigned long startMillis);

  // Do the normal work.
  void doWorkNormal();

  // Initializes the NTP.
  // returns the current time as epoch
  time_t initNTP();

  // Sends the measured values via MQTT.
  // parameters:
  //   resetReason ... the reset reason
  //   measured_values_bat ... the measured value for the battery
  //   measured_values_dht ... the measured value with MQTT
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
  void sendMQTTSleepTime(long sleepTimeInMicros);

  // Send the timings via MQTT.
  void sendMQTTTimings();

  // Calculates the time to sleep.
  // parameters:
  //   startMillis ... the milliseconds since the start of the program
  // returns the calculated sleep time in micro seconds
  long calculateSleepTimeMicros(unsigned long startMillis);

  // Dump the current log file buffer to the log file.
  void dumpLog();

  // Rotate the log files.
  void rotateLogs();

  // Renames the log file with the last good extension.
  void renameLogGood();

  // Renames the log file with the last bad extension.
  void renameLogLastBad();
};

}  // namespace arduino_temp