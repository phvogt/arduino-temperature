#include "logger.h"

#include "LittleFS.h"
#include "constants.h"

arduino_temp::Logger::Logger(NTP& ntp, boolean logEnabled) {
  logEnabled_ = logEnabled;
  ntp_ = &ntp;
}

void arduino_temp::Logger::setLogEnabled(boolean logEnabled) {
  logEnabled_ = logEnabled;
}

void arduino_temp::Logger::log(String logLevel, String message) {
  String logLine = ntp_->getDateTime() + " " + String(millis()) + " " +
                   logLevel + ": " + message;
  Serial.println(logLine);

  if (logEnabled_) {
    if (logBuffer_ == "") {
      logBuffer_ += logLine;
    } else {
      logBuffer_ += "\n" + logLine;
    }
  }
}

void arduino_temp::Logger::logInfo(String message) {
  log(LOGLEVEL_INFO, message);
}

void arduino_temp::Logger::logWarn(String message) {
  log(LOGLEVEL_WARN, message);
}

void arduino_temp::Logger::logError(String message) {
  log(LOGLEVEL_ERROR, message);
}

void arduino_temp::Logger::logTime(String message) {
  log(LOGLEVEL_TIME, message);
}

void arduino_temp::Logger::logInfoLine() {
  logInfo("------------------------------------------------------");
}

void arduino_temp::Logger::logInfoDoubleLine() {
  logInfo("======================================================");
}

String arduino_temp::Logger::getLogBuffer() { return logBuffer_; }
