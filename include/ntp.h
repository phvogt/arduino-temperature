#pragma once

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "config.h"
#include "constants.h"

namespace arduino_temp {

// Communication with an NTP server.
// Gets the current time.
class NTP {
 private:
  // WIFI UDP client
  WiFiUDP ntpUDP_;
  // NTP client to get the time
  NTPClient timeClient_;
  // current time
  time_t time = 0;

  // Get the NTP time epoch.
  // parameters:
  //   maxNtpRetries ... number of retries
  //   ntpRetryDelayInMillis ... delay in milliseconds between retries
  // returns the time as epoch
  time_t getNtpTime(int maxNtpRetries, unsigned long ntpRetryDelayInMillis);

  // Convert the int to a double digit string with leading zeroes.
  // parameters:
  //   value ... the value to convert
  // returns a string with the double digit value with a leading zero
  String toStringLeadingZero(int value);

 public:
  // Constructor.
  // parameters:
  //   ntpServer ... IP address of the NTP server
  //   ntpTimeOffset ... time offset in seconds
  //   ntpUpdateInterval ... update interval for NTP
  NTP(const char* ntpServer = NTP_SERVER, int ntpTimeOffset = NTP_TIME_OFFSET,
      int ntpUpdateInterval = NTP_UPDATE_INTERVAL);

  // Initialize the time from NTP.
  // resturns the current epoch
  time_t initNTP();

  // Gets the date/time in the format YY-MM-DD hh:mm:ss.
  // parameters: none
  // returns the date time as String
  String getDateTime();
};

}  // namespace arduino_temp
