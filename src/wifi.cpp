#include "wifi.h"

#include <ESP8266WiFi.h>

#include "constants.h"
#include "config.h"

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

struct RTCDATA rtcData;

arduino_temp::Wifi::Wifi(String ssid, String password,
                         int maxWifiConnectionRetries,
                         unsigned long wifiConnectRetryDelayInMillis,
                         int wifiConnectionRetryCountReset) : filehandler_(LOG_ENABLED) {
  // convert parameters
  password_ = (char *)malloc(password.length() + 1);
  password.toCharArray(password_, password.length() + 1);
  ssid_ = (char *)malloc(ssid.length() + 1);
  ssid.toCharArray(ssid_, ssid.length() + 1);

  maxWifiConnectionRetries_ = maxWifiConnectionRetries;
  wifiConnectRetryDelayInMillis_ = wifiConnectRetryDelayInMillis;
  wifiConnectionRetryCountReset_ = wifiConnectionRetryCountReset;
};

arduino_temp::Wifi::~Wifi() {
  free(password_);
  free(ssid_);
}

int arduino_temp::Wifi::initWifi() {
  // We start by connecting to a WiFi network

  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("init Wifi");

  filehandler_.doLogInfo("Wifi connecting to \"" + String(ssid_) + "\"");
  initWifiConfig();

  if (WIFI_RTC_ENABLED) {
    filehandler_.doLogInfo("connecting with RTC");
    boolean rtcValid = readWifiSettingsFromRTC();
    filehandler_.doLogInfo("  rtcValid: " + String(rtcValid));
    if (rtcValid) {
      // The RTC data was good, make a quick connection
      filehandler_.doLogInfo("  connecting with RTC-data");
      if (WiFi.status() != WL_CONNECTED) {
        filehandler_.doLogInfo("  not connected, calling begin()");
        WiFi.begin(ssid_, password_, rtcData.channel, rtcData.bssid, true);
      } else {
        filehandler_.doLogInfo("  already connected, not calling begin()");
      }
    } else {
      // The RTC data was not valid, so make a regular connection
      filehandler_.doLogInfo("  RTC invalid, connecting normally");
      WiFi.begin(ssid_, password_);
    }
  } else {
    filehandler_.doLogInfo("connecting without RTC");
    WiFi.begin(ssid_, password_);
  }

  int retryWifiCount = 1;

  long startMillis = millis();

  // test if already connected
  int connectionStatus = WiFi.status();
  while ((connectionStatus != WL_CONNECTED) &&
         (retryWifiCount <= maxWifiConnectionRetries_)) {
    filehandler_.doLogInfo("  #" + String(retryWifiCount) +
                           ", connectionStatus = " + String(connectionStatus));

    // reset WiFi
    if (retryWifiCount % wifiConnectionRetryCountReset_ == 0) {
      filehandler_.doLogInfo(
          "  disconnecting / config / new start of the connection");
      WiFi.disconnect();
      delay(10);
      WiFi.forceSleepBegin();
      delay(10);
      WiFi.forceSleepWake();
      delay(10);
      WiFi.persistent(false);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid_, password_);
    }

    retryWifiCount++;
    //    filehandler_.doLogInfo( "  #" + String(retryWifiCount) + " sleeping");
    delay(wifiConnectRetryDelayInMillis_);
    //    filehandler_.doLogInfo( "  #" + String(retryWifiCount) + " checking
    //    status");
    connectionStatus = WiFi.status();
  }

  // connected!
  if (connectionStatus == WL_CONNECTED) {
    filehandler_.doLogInfo("WiFi connected. storing Wifi settings in RTC");
    // store settings in RTC memory
    storeWifiSettingsInRTC();
    filehandler_.doLogInfo("WiFi connected. storing Wifi settings in RTC done");
    // log IP
    IPAddress localip = WiFi.localIP();
    String slocalip = localip.toString();
    filehandler_.doLogInfo("WiFi connected. IP address: " + slocalip);
  } else {
    filehandler_.doLogWarn("WiFi not connected! connectionStatus = " +
                           String(connectionStatus));
  }

  wifiMillis = millis() - startMillis;

  filehandler_.doLogInfoLine();

  return connectionStatus;
}

// Connect to Wifi.
// parameters: none
// returns connection status
boolean arduino_temp::Wifi::connectWifi() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("Connecting WIFI");

  // int wifiConnectionStatus = initWifi(WIFI_SSID, WIFI_SSID_PASSWORD,
  // 		WIFI_MAX_RETRIES, WIFI_CONNECT_RETRY_DELAY_IN_MILLIS,
  // 		WIFI_RETRY_RESET_COUNT);
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
  filehandler_.doLogInfo("initWifiConfig():");
  WiFi.forceSleepWake();
  delay(1);
  WiFi.persistent(false);
  boolean wifiStaSuccess = WiFi.mode(WIFI_STA);
  filehandler_.doLogInfo("  Wi-Fi mode set to WIFI_STA " +
                         String(wifiStaSuccess));

  // using static mode?
  if (WIFI_STATIC) {
    filehandler_.doLogInfo("using static connection");
    filehandler_.doLogInfo("  ip: " + String(WIFI_IP[0]) + "." +
                           String(WIFI_IP[1]) + "." + String(WIFI_IP[2]) + "." +
                           String(WIFI_IP[3]));
    filehandler_.doLogInfo("  dns: " + String(WIFI_DNS[0]) + "." +
                           String(WIFI_DNS[1]) + "." + String(WIFI_DNS[2]) +
                           "." + String(WIFI_DNS[3]));
    filehandler_.doLogInfo("  gateway: " + String(WIFI_GATEWAY[0]) + "." +
                           String(WIFI_GATEWAY[1]) + "." +
                           String(WIFI_GATEWAY[2]) + "." +
                           String(WIFI_GATEWAY[3]));
    filehandler_.doLogInfo(
        "  subnet: " + String(WIFI_SUBNET[0]) + "." + String(WIFI_SUBNET[1]) +
        "." + String(WIFI_SUBNET[2]) + "." + String(WIFI_SUBNET[3]));

    IPAddress ip(WIFI_IP[0], WIFI_IP[1], WIFI_IP[2], WIFI_IP[3]);
    IPAddress dns(WIFI_DNS[0], WIFI_DNS[1], WIFI_DNS[2], WIFI_DNS[3]);
    IPAddress gateway(WIFI_GATEWAY[0], WIFI_GATEWAY[1], WIFI_GATEWAY[2],
                      WIFI_GATEWAY[3]);
    IPAddress subnet(WIFI_SUBNET[0], WIFI_SUBNET[1], WIFI_SUBNET[2],
                     WIFI_SUBNET[3]);

    // from
    // https://www.bakke.online/index.php/2017/05/22/reducing-wifi-power-consumption-on-esp8266-part-3/
    WiFi.config(ip, gateway, subnet, dns, dns);
  } else {
    filehandler_.doLogInfo("using dhcp connection");
    WiFi.hostname(WIFI_HOSTNAME);
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
  filehandler_.doLogInfo("RTC reading");
  if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(rtcData))) {
    // Calculate the CRC of what we just read from RTC memory, but skip the
    // first 4 bytes as that's the checksum itself.
    uint32_t crc =
        calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
    filehandler_.doLogInfo("  read. crc = " + String(crc) +
                           " oldcrc = " + String(rtcData.crc32));
    if (crc == rtcData.crc32) {
      rtcValid = true;
      filehandler_.doLogInfo(
          "channel = " + String(rtcData.channel) +
          " BSSID = " + String(rtcData.bssid[0], HEX) + ":" +
          String(rtcData.bssid[1], HEX) + ":" + String(rtcData.bssid[2], HEX) +
          ":" + String(rtcData.bssid[3], HEX) + ":" +
          String(rtcData.bssid[4], HEX) + ":" + String(rtcData.bssid[5], HEX));
    }
  }

  filehandler_.doLogInfo("  rtcValid = " + String(rtcValid));

  return rtcValid;
}

void arduino_temp::Wifi::storeWifiSettingsInRTC() {
  // Write current connection info back to RTC
  rtcData.channel = WiFi.channel();
  memcpy(rtcData.bssid, WiFi.BSSID(),
         6);  // Copy 6 bytes of BSSID (AP's MAC address)
  rtcData.crc32 =
      calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
  ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData));

  filehandler_.doLogInfo(
      "channel = " + String(rtcData.channel) +
      " BSSID = " + String(rtcData.bssid[0], HEX) + ":" +
      String(rtcData.bssid[1], HEX) + ":" + String(rtcData.bssid[2], HEX) +
      ":" + String(rtcData.bssid[3], HEX) + ":" +
      String(rtcData.bssid[4], HEX) + ":" + String(rtcData.bssid[5], HEX));
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
