#pragma once

#include <Arduino.h>

namespace arduino_temp {

// Wifi configuration.
class WifiConfig {
 public:
  // SSID for Wifi
  const char* ssid;
  // password for Wifi
  const char* password;

  // use static wifi configuration (true) or DHCP (false)
  boolean isStatic;
  // store / retrieve wifi data from realtime clock (true) or not (false)
  boolean isRTCStoreEnabled;

  // host name for DHCP
  String hostname;
  // IP address for sensor
  byte* ip;
  // IP subnet for sensor
  byte* subnet;
  // IP address for gateway
  byte* gateway;

  // IP address for DNS server
  byte* dns;

  // only try this number times to connect to wifi (with delay below)
  int connectionMaxRetries;
  // delay between wifi connection retries
  unsigned long connectionRetryDelayInMillis;
  // after this number of retries the WiFi gets disconnected and reconnected
  int connectionRetryCountReset;
};

}  // namespace arduino_temp
