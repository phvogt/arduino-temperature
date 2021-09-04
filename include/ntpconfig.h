#pragma once

namespace arduino_temp {

// NTP configuration.
class NTPConfig {
 public:
  // NTP server name
  const char* server;
  // NTP time offset. Use 0 for UTC
  int timeOffset;
  // update interval for NTP
  int updateInterval;

  // only try this number times to connect to NTP (with delay below)
  int connectionMaxRetries;
  // delay between NTP connection retries in milli seconds
  unsigned long connectionRetryDelayInMillis;
};

}  // namespace arduino_temp
