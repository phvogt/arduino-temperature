#include "ntp.h"

#include <Arduino.h>
#include <TimeLib.h>

#include "config.h"
#include "constants.h"

arduino_temp::NTP::NTP(const char* ntpServer, int ntpTimeOffset,
                       int ntpUpdateInterval)
    : ntpUDP_(),
      timeClient_(ntpUDP_, ntpServer, ntpTimeOffset, ntpUpdateInterval) {
  ;
};

time_t arduino_temp::NTP::initNTP() {
  timeClient_.begin();
  time_t currentEpoch = getNtpTime(NTP_MAX_RETRIES, NTP_RETRY_DELAY_IN_MILLIS);
  return currentEpoch;
}

String arduino_temp::NTP::getDateTime() {
  return String(year()) + "-" + String(toStringLeadingZero(month())) + "-" +
         String(toStringLeadingZero(day())) + " " +
         String(toStringLeadingZero(hour())) + ":" +
         String(toStringLeadingZero(minute())) + ":" +
         String(toStringLeadingZero(second()));
}

String arduino_temp::NTP::toStringLeadingZero(int value) {
  if (value < 10) {
    return "0" + String(value);
  } else {
    return String(value);
  }
}

time_t arduino_temp::NTP::getNtpTime(int maxNtpRetries,
                                     unsigned long ntpRetryDelayInMillis) {
  timeClient_.update();

  int retryNtpCount = 1;

  unsigned long epoch = 0;

  epoch = timeClient_.getEpochTime();

  // values below 1000 are not realistic
  while ((epoch < 1000) && (retryNtpCount <= maxNtpRetries)) {
    retryNtpCount++;
    delay(ntpRetryDelayInMillis);
    timeClient_.update();
    epoch = timeClient_.getEpochTime();
  }

  return epoch;
}
