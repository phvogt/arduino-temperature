#pragma once

#include <Arduino.h>

#include "ntp.h"

namespace arduino_temp {

// Logger
class Logger {
 private:
  // NTP to get time for logging
  NTP* ntp_;

  // flag if the log should be enabled (true) or not (false)
  boolean logEnabled_ = false;
  // buffer for logs until the log is ready
  String logBuffer_ = "";

 public:
  // Constructor.
  // parameters:
  //   ntp ... NTP to get time
  //   logEnabled ... flag if the log should be enabled (true) or not (false)
  Logger(NTP& ntp, boolean logEnabled);

  // Sets if the log should be enabled (true) or not (false)
  // parameters:
  //   logEnabled ... whether the log should be enabled (true) or not (false)
  void setLogEnabled(boolean logEnabled);

  // Writes message with logLevel to serial and log file if global_logEnabled
  // is true. If global_logEnabled is false, the content is written to an
  // internal log buffer. Use dumpLogBuffer() to write the content to the log
  // file. parameters:
  //   logLevel ... log level
  //   message ... message to log
  // returns nothing
  void log(String logLevel, String message);

  // Logs the message with LOGLEVEL_INFO.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void logInfo(String message);

  // Logs the message with LOGLEVEL_WARN.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void logWarn(String message);

  // Logs the message with LOGLEVEL_ERROR.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void logError(String message);

  // Logs the message with LOGLEVEL_TIME.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void logTime(String message);

  // Logs a line LOGLEVEL_INFO.
  // parameters: none
  // returns nothing
  void logInfoLine();

  // Logs a double line LOGLEVEL_INFO.
  // parameters: none
  // returns nothing
  void logInfoDoubleLine();

  // get the log buffer
  // parameters: none
  // return the content of the log buffer.
  String getLogBuffer();
};

}  // namespace arduino_temp