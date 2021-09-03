#pragma once

#include <Arduino.h>

namespace arduino_temp {

// WIFI connection.
class Wifi {
 private:
  // contains the WIFI SSID.
  char *ssid_;
  // contains the WIFI password.
  char *password_;

  // maximum number of retries for the connection
  int maxWifiConnectionRetries_;
  // delay in milliseconds between connection retries.
  unsigned long wifiConnectRetryDelayInMillis_;
  // number of retries until the connection gets reset
  int wifiConnectionRetryCountReset_;

  // Configures the Wifi with the provided configuration
  void initWifiConfig();

  // Reads the Wifi settings data from RTC.
  // From
  // https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  // returns true if data could be read
  boolean readWifiSettingsFromRTC();

  // Stores the current Wifi-Settings in RTC.
  // From https:
  // //www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  void storeWifiSettingsInRTC();

  // Calculate the CRC32 value.
  // From https:
  // //www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  // parameters:
  //   data ... data to calculate the CRC32 value for
  //   length ... length of data
  // returns the CRC32 value
  uint32_t calculateCRC32(const uint8_t *data, size_t length);

 public:
  // milliseconds until wifi connection was established
  long wifiMillis = 0;

  // parameters:
  //   wifissid ... SSID
  //   wifipassword ... WIFI password
  //   maxWifiConnectionRetries ... number of connection retries
  //   wifiConnectRetryDelayInMillis ... delay in milliseconds between retries
  //   wifiConnectionRetryCountReset ... after this number of retries the
  //   connection gets reset
  Wifi(String ssid, String password, int maxWifiConnectionRetries,
       unsigned long wifiConnectRetryDelayInMillis,
       int wifiConnectionRetryCountReset);

  // free members
  ~Wifi();

  // Initialize WIFI.
  // returns the WiFi.status()
  int initWifi();

  // Connect to Wifi.
  boolean connectWifi();

  // Turn Wifi off. Should be called early in setup();
  void initWifiOff();

  // Stops Wifi for sleeping.
  // From
  // https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
  void stopWifiForSleep();
};

}  // namespace arduino_temp
