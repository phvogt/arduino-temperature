#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "constants.h"
#include "corefunctionsconfig.h"
#include "ftpconfig.h"
#include "measureconfig.h"
#include "mqttconfig.h"
#include "ntpconfig.h"
#include "wificonfig.h"

namespace arduino_temp {

class Config {
 public:
  CoreFunctionsConfig COREFUNCTIONS_CONFIG;
  FTPConfig FTP_CONFIG;
  MeasureConfig MEASURE_CONFIG;
  MQTTConfig MQTT_CONFIG;
  NTPConfig NTP_CONFIG;
  WifiConfig WIFI_CONFIG;

  // constructor
  Config();
};
}  // namespace arduino_temp

// should the logging to the file system be enabled (true) or not (false)
const boolean LOG_ENABLED = false;

// what the program should do, see constants.h for valid values
const int WORK_MODE = WORK_MODE_NORMAL;
// const int WORK_MODE = WORK_MODE_DELETE_LOGFILES;

// sleep time in microseconds
const long CORE_SLEEP_TIME_IN_MICROS = 300 * 1000 * 1000;

// ========================================================================
// use the following in config.cpp
//
// #include "config.h"
//
// #include <ESP8266WiFi.h>
//
// #include "constants.h"
//
// arduino_temp::Config::Config() {
// // ---------------------------------------------------
// // WIFI
//
// // SSID for Wifi
// WIFI_CONFIG.ssid = "WifiSSID";
// // password for Wifi
// WIFI_CONFIG.password = "WifiPassword";
// // use static wifi configuration (true) or DHCP (false)
// WIFI_CONFIG.isStatic = true;
// // store / retrieve wifi data from realtime clock (true) or not (false)
// WIFI_CONFIG.isRTCStoreEnabled = true;
//
// // host name for DHCP
// WIFI_CONFIG.hostname = "tempsensor";
//
// the settings below are only used with isStatic = true;
//
// // IP address for sensor
// WIFI_CONFIG.ip = new byte[4]{192, 168, 0, 101};
// // IP subnet for sensor
// WIFI_CONFIG.subnet = new byte[4]{255, 255, 255, 0};
// // IP address for gateway
// WIFI_CONFIG.gateway = new byte[4]{192, 168, 0, 1};
// // IP address for DNS server
// WIFI_CONFIG.dns = new byte[4]{192, 168, 0, 1};
//
// // only try this number times to connect to wifi (with delay below)
// WIFI_CONFIG.connectionMaxRetries = WIFI_MAX_RETRIES;
// // delay between wifi connection retries in milli seconds
// WIFI_CONFIG.connectionRetryDelayInMillis = WIFI_RETRY_DELAY_IN_MILLIS;
// // after this number of retries the WiFi gets disconnected and reconnected
// WIFI_CONFIG.connectionRetryCountReset = WIFI_RETRY_RESET_COUNT;
//
// // ---------------------------------------------------
// // MQTT
//
// // MQTT host name
// MQTT_CONFIG.host = "192.168.0.1";
// // MQTT host port
// MQTT_CONFIG.port = 1883;
// // MQTT username
// MQTT_CONFIG.username = "mqttUsername";
// // MQTT password
// MQTT_CONFIG.password = "mqttPassword";
// // the topic to use in the MQTT message
// MQTT_CONFIG.topic = "mqttroom";
//
// // only try this number times to connect to MQTT (with delay below)
// MQTT_CONFIG.connectionMaxRetries = MQTT_MAX_RETRIES;
// // delay between MQTT connection retries in milli seconds
// MQTT_CONFIG.connectionRetryDelayInMillis = MQTT_RETRY_DELAY_IN_MILLIS;
//
// // ---------------------------------------------------
// // NTP
//
// // NTP server name
// NTP_CONFIG.server = "192.168.0.1";
// // NTP time offset. Use 0 for UTC
// NTP_CONFIG.timeOffset = 0;
// // update interval for NTP
// NTP_CONFIG.updateInterval = NTP_UPDATE_INTERVAL;
//
// // only try this number times to connect to NTP (with delay below)
// NTP_CONFIG.connectionMaxRetries = NTP_MAX_RETRIES;
// // delay between NTP connection retries in milli seconds
// NTP_CONFIG.connectionRetryDelayInMillis = NTP_RETRY_DELAY_IN_MILLIS;
//
// // ---------------------------------------------------
// // FTP
//
// // FTP server IP address
// FTP_CONFIG.server = FTPConfig(192, 168, 0, 1);
// // FTP server port
// FTP_CONFIG.port = 21;
// // FTP server username
// FTP_CONFIG.username = "ftpUser";
// // FTP server password
// FTP_CONFIG.password = "ftpPassword";
//
// // ---------------------------------------------------
// // Measure
//
// // type of DHT
// MEASURE_CONFIG.dhtType = DHTTYPE;
// // pin the DHT is connected
// MEASURE_CONFIG.dhtPin = DHTPIN;
// // flag if values should be simulated (true) or not (false)
// MEASURE_CONFIG.isSimulateValuesEnabled = DHT_SIMULATE_VALUES;
// // max retries to read values
// MEASURE_CONFIG.dhtMaxRetries = DHT_MAX_READ_VALUES_RETRIES;
// // delay in milliseconds between retries
// MEASURE_CONFIG.dhtRetryDelayInMillis = DHT_DELAY_IN_MILLIS;
//
// // value of the resistor(s) from ADC to the battery
// const double BATTERY_RESISTOR_ADC_BATT_VALUE = 220000 + 470000;
// // value of the resistor(s) from ADC to the ground
// const double BATTERY_RESISTOR_ADC_GND_VALUE = 100000;
// // factor for calculating the voltage of the battery from the measured value
// MEASURE_CONFIG.batteryFactor =  (BATTERY_RESISTOR_ADC_GND_VALUE +
// BATTERY_RESISTOR_ADC_BATT_VALUE) / BATTERY_RESISTOR_ADC_GND_VALUE)
//
// }