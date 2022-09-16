#include "wifi.h"

#include <ESP8266WiFi.h>

arduino_temp::Wifi::Wifi(WifiConfig& wifiConfig, Logger& logger) {
  wifiConfig_ = &wifiConfig;
  logger_  = &logger;
};

int arduino_temp::Wifi::initWifi() {
  // We start by connecting to a WiFi network

  logger_->logInfoLine();
  logger_->logInfo("init Wifi");

  logger_->logInfo("Wifi connecting to \"" + String(wifiConfig_->ssid) + "\"");
  initWifiConfig();

  if (wifiConfig_->isRTCStoreEnabled) {
    logger_->logInfo("connecting with RTC");
    boolean rtcValid = readWifiSettingsFromRTC();
    logger_->logInfo("  rtcValid: " + String(rtcValid));
    if (rtcValid) {
      // The RTC data was good, make a quick connection
      logger_->logInfo("  connecting with RTC-data");
      if (WiFi.status() != WL_CONNECTED) {
        logger_->logInfo("  not connected, calling begin()");
        WiFi.begin(wifiConfig_->ssid, wifiConfig_->password, rtcData_.channel,
                   rtcData_.bssid, true);
      } else {
        logger_->logInfo("  already connected, not calling begin()");
      }
    } else {
      // The RTC data was not valid, so make a regular connection
      logger_->logInfo("  RTC invalid, connecting normally");
      WiFi.begin(wifiConfig_->ssid, wifiConfig_->password);
    }
  } else {
    logger_->logInfo("connecting without RTC");
    WiFi.begin(wifiConfig_->ssid, wifiConfig_->password);
  }

  int retryWifiCount = 1;

  long startMillis = millis();

  // test if already connected
  int connectionStatus = WiFi.status();
  while ((connectionStatus != WL_CONNECTED) &&
         (retryWifiCount <= wifiConfig_->connectionMaxRetries)) {
    logger_->logInfo("  #" + String(retryWifiCount) +
                    ", connectionStatus = " + String(connectionStatus));

    // reset WiFi
    if (retryWifiCount % wifiConfig_->connectionRetryCountReset == 0) {
      logger_->logInfo("  disconnecting / config / new start of the connection");
      WiFi.disconnect();
      delay(10);
      WiFi.forceSleepBegin();
      delay(10);
      WiFi.forceSleepWake();
      delay(10);
      WiFi.persistent(false);
      WiFi.mode(WIFI_STA);
      WiFi.begin(wifiConfig_->ssid, wifiConfig_->password);
    }

    retryWifiCount++;
    //    logger_->logInfo( "  #" + String(retryWifiCount) +
    //    " sleeping");
    delay(wifiConfig_->connectionRetryDelayInMillis);
    //    logger_->logInfo( "  #" + String(retryWifiCount) +
    //    " checking status");
    connectionStatus = WiFi.status();
  }

  // connected!
  if (connectionStatus == WL_CONNECTED) {
    logger_->logInfo("WiFi connected. storing Wifi settings in RTC");
    // store settings in RTC memory
    storeWifiSettingsInRTC();
    logger_->logInfo("WiFi connected. storing Wifi settings in RTC done");
    // log IP
    IPAddress localip = WiFi.localIP();
    String slocalip = localip.toString();
    logger_->logInfo("WiFi connected. IP address: " + slocalip);
  } else {
    logger_->logWarn("WiFi not connected! connectionStatus = " +
                    String(connectionStatus));
  }

  wifiMillis = millis() - startMillis;

  logger_->logInfoLine();

  return connectionStatus;
}

// Connect to Wifi.
// parameters: none
// returns connection status
boolean arduino_temp::Wifi::connectWifi() {
  logger_->logInfoLine();
  logger_->logInfo("Connecting WIFI");

  // int wifiConnectionStatus = initWifi(WIFI_SSID,
  // WIFI_wifiConfig_->ssidPASSWORD, 		WIFI_MAX_RETRIES,
  // WIFI_CONNECT_RETRY_DELAY_IN_MILLIS, 		WIFI_RETRY_RESET_COUNT);
  int wifiConnectionStatus = initWifi();
  boolean connected = (wifiConnectionStatus == WL_CONNECTED);

  return connected;
}

// Turn Wifi off. Should be called early in setup();
// parameters: none
// returns nothing
void arduino_temp::Wifi::initWifiOff() {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
}

// Configures the WiFi.
// parameters: none
// returns nothing
void arduino_temp::Wifi::initWifiConfig() {
  logger_->logInfo("initWifiConfig():");
  WiFi.forceSleepWake();
  delay(1);
  WiFi.persistent(false);
  boolean wifiStaSuccess = WiFi.mode(WIFI_STA);
  logger_->logInfo("  Wi-Fi mode set to WIFI_STA " + String(wifiStaSuccess));

  // using static mode?
  if (wifiConfig_->isStatic) {
    logger_->logInfo("using static connection");
    logger_->logInfo(
        "  ip: " + String(wifiConfig_->ip[0]) + "." + String(wifiConfig_->ip[1]) +
        "." + String(wifiConfig_->ip[2]) + "." + String(wifiConfig_->ip[3]));
    logger_->logInfo("  dns: " + String(wifiConfig_->dns[0]) + "." +
                    String(wifiConfig_->dns[1]) + "." +
                    String(wifiConfig_->dns[2]) + "." +
                    String(wifiConfig_->dns[3]));
    logger_->logInfo("  gateway: " + String(wifiConfig_->gateway[0]) + "." +
                    String(wifiConfig_->gateway[1]) + "." +
                    String(wifiConfig_->gateway[2]) + "." +
                    String(wifiConfig_->gateway[3]));
    logger_->logInfo("  subnet: " + String(wifiConfig_->subnet[0]) + "." +
                    String(wifiConfig_->subnet[1]) + "." +
                    String(wifiConfig_->subnet[2]) + "." +
                    String(wifiConfig_->subnet[3]));

    IPAddress ip(wifiConfig_->ip[0], wifiConfig_->ip[1], wifiConfig_->ip[2],
                 wifiConfig_->ip[3]);
    IPAddress dns(wifiConfig_->dns[0], wifiConfig_->dns[1], wifiConfig_->dns[2],
                  wifiConfig_->dns[3]);
    IPAddress gateway(wifiConfig_->gateway[0], wifiConfig_->gateway[1],
                      wifiConfig_->gateway[2], wifiConfig_->gateway[3]);
    IPAddress subnet(wifiConfig_->subnet[0], wifiConfig_->subnet[1],
                     wifiConfig_->subnet[2], wifiConfig_->subnet[3]);

    // from
    // https://www.bakke.online/index.php/2017/05/22/reducing-wifi-power-consumption-on-esp8266-part-3/
    WiFi.config(ip, gateway, subnet, dns, dns);
  } else {
    logger_->logInfo("using dhcp connection");
    WiFi.hostname(wifiConfig_->hostname);
  }
}

void arduino_temp::Wifi::stopWifiForSleep() {
  WiFi.disconnect(true);
  delay(1);
  WiFi.mode(WIFI_OFF);
}

boolean arduino_temp::Wifi::readWifiSettingsFromRTC() {
  // Try to read WiFi settings from RTC memory
  boolean rtcValid = false;
  logger_->logInfo("RTC reading");
  if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData_, sizeof(rtcData_))) {
    // Calculate the CRC of what we just read from RTC memory, but skip the
    // first 4 bytes as that's the checksum itself.
    uint32_t crc =
        calculateCRC32(((uint8_t *)&rtcData_) + 4, sizeof(rtcData_) - 4);
    logger_->logInfo("  read. crc = " + String(crc) +
                    " oldcrc = " + String(rtcData_.crc32));
    if (crc == rtcData_.crc32) {
      rtcValid = true;
      logger_->logInfo("channel = " + String(rtcData_.channel) +
                      " BSSID = " + String(rtcData_.bssid[0], HEX) + ":" +
                      String(rtcData_.bssid[1], HEX) + ":" +
                      String(rtcData_.bssid[2], HEX) + ":" +
                      String(rtcData_.bssid[3], HEX) + ":" +
                      String(rtcData_.bssid[4], HEX) + ":" +
                      String(rtcData_.bssid[5], HEX));
    }
  }

  logger_->logInfo("  rtcValid = " + String(rtcValid));

  return rtcValid;
}

void arduino_temp::Wifi::storeWifiSettingsInRTC() {
  // Write current connection info back to RTC
  rtcData_.channel = WiFi.channel();
  memcpy(rtcData_.bssid, WiFi.BSSID(),
         6);  // Copy 6 bytes of BSSID (AP's MAC address)
  rtcData_.crc32 =
      calculateCRC32(((uint8_t *)&rtcData_) + 4, sizeof(rtcData_) - 4);
  ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData_, sizeof(rtcData_));

  logger_->logInfo(
      "channel = " + String(rtcData_.channel) +
      " BSSID = " + String(rtcData_.bssid[0], HEX) + ":" +
      String(rtcData_.bssid[1], HEX) + ":" + String(rtcData_.bssid[2], HEX) +
      ":" + String(rtcData_.bssid[3], HEX) + ":" +
      String(rtcData_.bssid[4], HEX) + ":" + String(rtcData_.bssid[5], HEX));
}

uint32_t arduino_temp::Wifi::calculateCRC32(const uint8_t *data,
                                            size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }

      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}
