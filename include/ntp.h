#pragma once

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "ntpconfig.h"

namespace arduino_temp {

// Communication with an NTP server.
// Gets the current time.
class NTP {
 private:
  // NTP configuration
  NTPConfig ntpConfig_;

  // WIFI UDP client
  WiFiUDP ntpUDP_;
  // NTP client to get the time
  NTPClient timeClient_;
  // current time
  time_t time = 0;

  // Get the NTP time epoch.
  // parameters: none
  // returns the time as epoch
  time_t getNtpTime();

  // Convert the int to a double digit string with leading zeroes.
  // parameters:
  //   value ... the value to convert
  // returns a string with the double digit value with a leading zero
  String toStringLeadingZero(int value);

 public:
  // Constructor.
  // parameters:
  //   ntpConfig ... NTP configuration
  NTP(NTPConfig ntpConfig);

  // Initialize the time from NTP.
  // parameters: none
  // resturns the current epoch
  time_t initNTP();

  // Gets the date/time in the format YY-MM-DD hh:mm:ss.
  // parameters: none
  // returns the date time as String
  String getDateTime();
};

}  // namespace arduino_temp
