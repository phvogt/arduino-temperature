#include "corefunctions.h"

#include "config.h"

// Get the reset reason.
// parameters: none
// returns the reset reason
String arduino_temp::CoreFunctions::determineResetReason() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Getting reset reason");

  // get the reason the esp was reset
  String resetReason = ESP.getResetReason();
  FileHandler::getInstance().doLogInfo("resetReason: " + resetReason);
  FileHandler::getInstance().doLogInfoLine();

  return resetReason;
}

long arduino_temp::CoreFunctions::calcSleepTimeInMicroSeconds(
    int sleepTimeInMillis, int startTimeInMillis) {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("calculating sleep time");

  // calculate the already worked millis
  long currentMillis = millis();
  long workedTimeMillis = currentMillis - startTimeInMillis;
  if (workedTimeMillis < 0) {
    workedTimeMillis = 0;
  }

  long sleepTimeMicro =
      (sleepTimeInMillis * 1000 - workedTimeMillis * 1000) * DEEP_SLEEP_FACTOR;
  FileHandler::getInstance().doLogInfo("startTimeInMillis: " + String(startTimeInMillis) +
                         " sleepTimeInMillis: " + String(sleepTimeInMillis) +
                         " sleepFactor = " + String(DEEP_SLEEP_FACTOR) +
                         " sleepTimeMicro: " + String(sleepTimeMicro));
  FileHandler::getInstance().doLogInfoLine();

  return sleepTimeMicro;
}

void arduino_temp::CoreFunctions::doSleepForMicros(long sleepTimeInMicros) {
  ESP.deepSleep(sleepTimeInMicros, WAKE_RF_DISABLED);
  delay(100);
}

void arduino_temp::CoreFunctions::sleepMaxTime() {
  int sleepTimeMicro = CORE_SLEEP_TIME_IN_MILLIS * 1000;
  FileHandler::getInstance().doLogInfo("going max time sleeping for " +
                         String(sleepTimeMicro));
  doSleepForMicros(sleepTimeMicro);
}
