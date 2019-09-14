#include "all_headers.h"

long global_wifiMillis = 0;

// from https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct RTCDATA{
  uint32_t crc32;   // 4 bytes
  uint8_t channel;  // 1 byte,   5 in total
  uint8_t bssid[6]; // 6 bytes, 11 in total
  uint8_t padding;  // 1 byte,  12 in total
};

struct RTCDATA rtcData;

// Initialize WIFI.
// parameters:
//   wifissid ... SSID
//   wifipassword ... WIFI password
//   maxWifiConnectionRetries ... number of connection retries
//   wifiConnectRetryDelayInMillis ... delay in milliseconds between retries
//   wifiConnectionRetryCountReset ... after this number of retries the connection gets reset
// returns the WiFi.status()
int initWifi(String ssid, String password, int maxWifiConnectionRetries,
		unsigned long wifiConnectRetryDelayInMillis,
		int wifiConnectionRetryCountReset) {

	// convert parameters
	char c_password[password.length() + 1];
	password.toCharArray(c_password, password.length() + 1);
	char c_ssid[ssid.length() + 1];
	ssid.toCharArray(c_ssid, ssid.length() + 1);

	// We start by connecting to a WiFi network

	doLogInfo("------------------------------------------------------");
	doLogInfo("init Wifi");

	doLogInfo("Wifi connecting to \"" + ssid + "\"");
	initWifiConfig();

	if (WIFI_RTC_ENABLED) {
		doLogInfo("connecting with RTC");
		boolean rtcValid = readWifiSettingsFromRTC();
		doLogInfo("  rtcValid: " + String(rtcValid));
		if (rtcValid) {
			// The RTC data was good, make a quick connection
			doLogInfo("  connecting with RTC-data");
			if (WiFi.status() != WL_CONNECTED) {
				doLogInfo("  not connected, calling begin()");
				WiFi.begin(c_ssid, c_password, rtcData.channel, rtcData.bssid,
						true);
			} else {
				doLogInfo("  already connected, not calling begin()");
			}
		} else {
			// The RTC data was not valid, so make a regular connection
			doLogInfo("  RTC invalid, connecting normally");
			WiFi.begin(c_ssid, c_password);
		}
	} else {
		doLogInfo("connecting without RTC");
		WiFi.begin(c_ssid, c_password);
	}

	int retryWifiCount = 1;

	long startMillis = millis();

	// test if already connected
	int connectionStatus = WiFi.status();
	while ((connectionStatus != WL_CONNECTED)
			&& (retryWifiCount <= maxWifiConnectionRetries)) {
		doLogInfo(
				"  #" + String(retryWifiCount) + ", connectionStatus = "
						+ String(connectionStatus));

		// reset WiFi
		if (retryWifiCount % wifiConnectionRetryCountReset == 0) {
			doLogInfo("  disconnecting / config / new start of the connection");
			WiFi.disconnect();
			delay(10);
			WiFi.forceSleepBegin();
			delay(10);
			WiFi.forceSleepWake();
			delay(10);
			WiFi.persistent(false);
			WiFi.mode(WIFI_STA);
			WiFi.begin(c_ssid, c_password);
		}

		retryWifiCount++;
		//    doLogInfo( "  #" + String(retryWifiCount) + " sleeping");
		delay(wifiConnectRetryDelayInMillis);
		//    doLogInfo( "  #" + String(retryWifiCount) + " checking status");
		connectionStatus = WiFi.status();
	}

	// connected!
	if (connectionStatus == WL_CONNECTED) {
		doLogInfo("WiFi connected. storing Wifi settings in RTC");
		// store settings in RTC memory
		storeWifiSettingsInRTC();
		doLogInfo("WiFi connected. storing Wifi settings in RTC done");
		// log IP
		IPAddress localip = WiFi.localIP();
		String slocalip = localip.toString();
		doLogInfo("WiFi connected. IP address: " + slocalip);
	} else {
		doLogWarn(
				"WiFi not connected! connectionStatus = "
						+ String(connectionStatus));
	}

	global_wifiMillis = millis() - startMillis;

	doLogInfo("------------------------------------------------------");

	return connectionStatus;
}

// Turn Wifi off. Should be called early in setup();
// parameters: none
// returns nothing
void initWifiOff() {
	WiFi.mode(WIFI_OFF);
	WiFi.forceSleepBegin();
	delay(1);
}

// Configures the WiFi.
// parameters: none
// returns nothing
void initWifiConfig() {

	doLogInfo("initWifiConfig():");
	WiFi.forceSleepWake();
	delay(1);
	WiFi.persistent(false);
	boolean wifiStaSuccess = WiFi.mode(WIFI_STA);
	doLogInfo("  Wi-Fi mode set to WIFI_STA " + String(wifiStaSuccess));

	// using static mode?
	if (WIFI_STATIC) {

		doLogInfo("using static connection");
		doLogInfo(
				"  ip: " + String(WIFI_IP[0]) + "." + String(WIFI_IP[1]) + "."
						+ String(WIFI_IP[2]) + "." + String(WIFI_IP[3]));
		doLogInfo(
				"  dns: " + String(WIFI_DNS[0]) + "." + String(WIFI_DNS[1])
						+ "." + String(WIFI_DNS[2]) + "."
						+ String(WIFI_DNS[3]));
		doLogInfo(
				"  gateway: " + String(WIFI_GATEWAY[0]) + "."
						+ String(WIFI_GATEWAY[1]) + "."
						+ String(WIFI_GATEWAY[2]) + "."
						+ String(WIFI_GATEWAY[3]));
		doLogInfo(
				"  subnet: " + String(WIFI_SUBNET[0]) + "."
						+ String(WIFI_SUBNET[1]) + "." + String(WIFI_SUBNET[2])
						+ "." + String(WIFI_SUBNET[3]));

		IPAddress ip(WIFI_IP[0], WIFI_IP[1], WIFI_IP[2], WIFI_IP[3]);
		IPAddress dns(WIFI_DNS[0], WIFI_DNS[1], WIFI_DNS[2], WIFI_DNS[3]);
		IPAddress gateway(WIFI_GATEWAY[0], WIFI_GATEWAY[1], WIFI_GATEWAY[2],
				WIFI_GATEWAY[3]);
		IPAddress subnet(WIFI_SUBNET[0], WIFI_SUBNET[1], WIFI_SUBNET[2],
				WIFI_SUBNET[3]);

		// from https://www.bakke.online/index.php/2017/05/22/reducing-wifi-power-consumption-on-esp8266-part-3/
		WiFi.config(ip, gateway, subnet, dns, dns);

	} else {
		doLogInfo("using dhcp connection");
		WiFi.hostname(WIFI_HOSTNAME);
	}

}

// Stops Wifi for sleeping.
// From https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// parameters: none
// returns nothing
void stopWifiForSleep() {
	WiFi.disconnect(true);
	delay(1);
	WiFi.mode(WIFI_OFF);
}

// Reads the Wifi settings data from RTC.
// From https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// parameters: none
// returns true if data could be read
boolean readWifiSettingsFromRTC() {

	// Try to read WiFi settings from RTC memory
	boolean rtcValid = false;
	doLogInfo("RTC reading");
	if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
		// Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
		uint32_t crc = calculateCRC32(((uint8_t*) &rtcData) + 4,
				sizeof(rtcData) - 4);
		doLogInfo(
				"  read. crc = " + String(crc) + " oldcrc = "
						+ String(rtcData.crc32));
		if (crc == rtcData.crc32) {
			rtcValid = true;
			doLogInfo(
					"channel = " + String(rtcData.channel) + " BSSID = "
							+ String(rtcData.bssid[0], HEX) + ":"
							+ String(rtcData.bssid[1], HEX) + ":"
							+ String(rtcData.bssid[2], HEX) + ":"
							+ String(rtcData.bssid[3], HEX) + ":"
							+ String(rtcData.bssid[4], HEX) + ":"
							+ String(rtcData.bssid[5], HEX));
		}
	}

	doLogInfo("  rtcValid = " + String(rtcValid));

	return rtcValid;
}

// Stores the current Wifi-Settings in RTC.
// From https: //www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// parameters: none
// returns nothing
void storeWifiSettingsInRTC() {
	// Write current connection info back to RTC
	rtcData.channel = WiFi.channel();
	memcpy(rtcData.bssid, WiFi.BSSID(), 6);// Copy 6 bytes of BSSID (AP's MAC address)
	rtcData.crc32 = calculateCRC32(((uint8_t*) &rtcData) + 4,
			sizeof(rtcData) - 4);
	ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData));

	doLogInfo(
			"channel = " + String(rtcData.channel) + " BSSID = "
			+ String(rtcData.bssid[0], HEX) + ":"
			+ String(rtcData.bssid[1], HEX) + ":"
			+ String(rtcData.bssid[2], HEX) + ":"
			+ String(rtcData.bssid[3], HEX) + ":"
			+ String(rtcData.bssid[4], HEX) + ":"
			+ String(rtcData.bssid[5], HEX));
}

// Calculate the CRC32 value.
// From https: //www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// parameters:
//   data ... data to calculate the CRC32 value for
//   length ... length of data
// returns the CRC32 value
uint32_t calculateCRC32(const uint8_t *data, size_t length) {
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
