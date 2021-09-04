#pragma once

#include <Arduino.h>

#include "logger.h"
#include "wificonfig.h"

// from
// https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods
// read and write 4 bytes at a time, so the RTC data structure should be padded
// to a 4-byte multiple.
struct RTCDATA {
  uint32_t crc32;    // 4 bytes
  uint8_t channel;   // 1 byte,   5 in total
  uint8_t bssid[6];  // 6 bytes, 11 in total
  uint8_t padding;   // 1 byte,  12 in total
};

namespace arduino_temp {

// WIFI connection.
class Wifi {
 private:
  // Wifi configuration
  WifiConfig wifiConfig_;
  // logging
  Logger logger_;

  struct RTCDATA rtcData_;

  // Configures the Wifi with the provided configuration
  // parameters: none
  // returns nothing
  void initWifiConfig();

  // Reads the Wifi settings data from RTC.
  // From
  // https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  // parameters: none
  // returns true if data could be read
  boolean readWifiSettingsFromRTC();

  // Stores the current Wifi-Settings in RTC.
  // From https:
  // //www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  // parameters: none
  // returns nothing
  void storeWifiSettingsInRTC();

  // Calculate the CRC32 value.
  // From https:
  // //www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  // parameters:
  //   data ... data to calculate the CRC32 value for
  //   length ... length of data
  // returns the CRC32 value
  uint32_t calculateCRC32(const uint8_t* data, size_t length);

 public:
  // milliseconds until wifi connection was established
  long wifiMillis = 0;

  // Constructor.
  // parameters:
  //   wifiConfig ... Wifi configuration
  //   logger ... logging
  Wifi(WifiConfig wifiConfig, Logger logger);

  // Initialize WIFI.
  // parameters: none
  // returns the WiFi.status()
  int initWifi();

  // Connect to Wifi.
  // parameters: none
  // returns if the connections succeeded
  boolean connectWifi();

  // Turn Wifi off. Should be called early in setup();
  // parameters: none
  // returns nothing
  void initWifiOff();

  // Stops Wifi for sleeping.
  // From
  // https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  // parameters: none
  // returns nothing
  void stopWifiForSleep();
};

}  // namespace arduino_temp
