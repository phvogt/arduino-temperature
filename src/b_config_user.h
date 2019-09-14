#ifndef CONFIG_USER_H_
#define CONFIG_USER_H_

#include <Arduino.h>

// ---------------------------------------------------
// specific configuration
const int CORE_SLEEP_TIME_IN_MILLIS = 300000;

const String WIFI_SSID = "vlbg";
const String WIFI_SSID_PASSWORD = "Czox8iGzGr6Wv8ZtqzhL";
const boolean WIFI_STATIC = true;
const byte WIFI_DNS[4] = { 192, 168, 2, 2 };
const byte WIFI_GATEWAY[4] = { 192, 168, 2, 2 };
const byte WIFI_SUBNET[4] = { 255, 255, 255, 0 };
const String MQTT_HOST = "192.168.2.2";
const int MQTT_PORT = 1883;
const String MQTT_USERNAME = "temp";
const String MQTT_PASSWORD = "3r3ufPqE4Q";
static const char* NTP_SERVER = "192.168.2.2";
const int NTP_TIME_OFFSET = 0;
const String FTP_USERNAME = "vogge";
const String FTP_PASSWORD = "Srmnq8US";

// test sensor
//const double BATTERY_RESISTOR_ADC_BATT_VALUE = 220000 + 470000;
//const double BATTERY_RESISTOR_ADC_GND_VALUE = 100000;
//const String WIFI_HOSTNAME = "sensor-test";
//const String MQTT_ROOM = "room_test";
//const byte WIFI_IP[4] = { 192, 168, 2, 102 };

//// sensor Arbeitszimmer 001
//const double BATTERY_RESISTOR_ADC_BATT_VALUE = 220000 + 470000;
//const double BATTERY_RESISTOR_ADC_GND_VALUE = 100000;
//const String WIFI_HOSTNAME = "sensor-az";
//const String MQTT_ROOM = "room_arbeitszimmer";
//const byte WIFI_IP[4] = {192, 168, 2, 100};

// Sensor aussen 002
const double BATTERY_RESISTOR_ADC_BATT_VALUE = 215000 + 456000;
const double BATTERY_RESISTOR_ADC_GND_VALUE = 97900;
const String WIFI_HOSTNAME = "sensor-test";
const String MQTT_ROOM = "room_test";
const byte WIFI_IP[4] = { 192, 168, 2, 101 };

#endif /* CONFIG_USER_H_ */