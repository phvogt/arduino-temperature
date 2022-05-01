#include "timing.h"

arduino_temp::Timing::Timing(Logger& logger) {
  logger_ = &logger;
}

void arduino_temp::Timing::timingStart() { timingStart_ = millis(); }

unsigned long arduino_temp::Timing::timingEnd(String measuredName) {
  unsigned long duration = millis() - timingStart_;
  String logstring = measuredName + ": " + String(duration);

  logger_->logTime(logstring);

  if (timings_ == "") {
    timings_ += logstring;
  } else {
    timings_ += "," + logstring;
  }

  return duration;
}

String arduino_temp::Timing::getTimings() { return timings_; }