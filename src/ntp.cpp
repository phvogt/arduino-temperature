#include "ntp.h"

#include <Arduino.h>
#include <TimeLib.h>

arduino_temp::NTP::NTP(NTPConfig ntpConfig)
    : ntpConfig_(ntpConfig),
      ntpUDP_(),
      timeClient_(ntpUDP_, ntpConfig.server, ntpConfig.timeOffset,
                  ntpConfig.updateInterval){};

time_t arduino_temp::NTP::initNTP() {
  timeClient_.begin();
  time_t currentEpoch = getNtpTime();
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

time_t arduino_temp::NTP::getNtpTime() {
  timeClient_.update();

  int retryNtpCount = 1;

  unsigned long epoch = 0;

  epoch = timeClient_.getEpochTime();

  // values below 1000 are not realistic
  while ((epoch < 1000) && (retryNtpCount <= ntpConfig_.connectionMaxRetries)) {
    retryNtpCount++;
    delay(ntpConfig_.connectionRetryDelayInMillis);
    timeClient_.update();
    epoch = timeClient_.getEpochTime();
  }

  return epoch;
}
