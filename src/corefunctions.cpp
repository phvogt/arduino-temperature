#include "corefunctions.h"

arduino_temp::CoreFunctions::CoreFunctions(
    CoreFunctionsConfig coreFunctionsConfig, Logger logger)
    : coreFunctionsConfig_(coreFunctionsConfig), logger_(logger) {}

String arduino_temp::CoreFunctions::determineResetReason() {
  logger_.logInfoLine();
  logger_.logInfo("Getting reset reason");

  // get the reason the esp was reset
  String resetReason = ESP.getResetReason();
  logger_.logInfo("resetReason: " + resetReason);
  logger_.logInfoLine();

  return resetReason;
}

long arduino_temp::CoreFunctions::calcSleepTimeInMicroSeconds(
    int sleepTimeInMicros, int startTimeInMicros) {
  logger_.logInfoLine();
  logger_.logInfo("calculating sleep time");

  // calculate the already worked millis
  long currentMillis = millis();
  long workedTimeMicros = currentMillis * 1000 - startTimeInMicros;
  if (workedTimeMicros < 0) {
    workedTimeMicros = 0;
  }

  long sleepTimeMicro = (sleepTimeInMicros - workedTimeMicros) *
                        coreFunctionsConfig_.deepSleepFactor;
  logger_.logInfo(
      "startTimeInMicros: " + String(startTimeInMicros) +
      " sleepTimeInMicros: " + String(sleepTimeInMicros) +
      " workedTimeMicros: " + String(workedTimeMicros) +
      " sleepFactor = " + String(coreFunctionsConfig_.deepSleepFactor) +
      " sleepTimeMicro: " + String(sleepTimeMicro));
  logger_.logInfoLine();

  return sleepTimeMicro;
}

void arduino_temp::CoreFunctions::doSleepForMicros(long sleepTimeInMicros) {
  ESP.deepSleep(sleepTimeInMicros, WAKE_RF_DISABLED);
  delay(100);
}
