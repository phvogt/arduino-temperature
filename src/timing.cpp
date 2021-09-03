#include "timing.h"

#include "constants.h"
#include "filehandler.h"

arduino_temp::Timing::Timing() {}

void arduino_temp::Timing::timingStart() { timingStart_ = millis(); }

unsigned long arduino_temp::Timing::timingEnd(String measuredName) {
  unsigned long duration = millis() - timingStart_;
  String logstring = measuredName + ": " + String(duration);

  FileHandler::getInstance().doLogTime(logstring);

  if (timings_ == "") {
    timings_ += logstring;
  } else {
    timings_ += "," + logstring;
  }

  return duration;
}

String arduino_temp::Timing::getTimings() { return timings_; }