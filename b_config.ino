// ---------------------------------------------------
// common configuration for every sensor in a network

// ---------------------------------------------------
// e_core_functions.ino constants

// sleep time in milliseconds
const int CORE_SLEEP_TIME_IN_MILLIS = 60000;

// ---------------------------------------------------
// f_wifi.ino constants

// SSID for Wifi
const String WIFI_SSID = "WifiSSID";
// password for Wifi
const String WIFI_SSID_PASSWORD = "WifiPassword";

// use static wifi configuration -> true
// for DHCP set to false
const boolean WIFI_STATIC = true;

// the settings below are only used with WIFI_STATIC = true;
// IP address for sensor
const byte WIFI_IP[4] = { 192, 168, 0, 101 };
// IP address for DNS server
const byte WIFI_DNS[4] = { 192, 168, 0, 1 };
// IP address for gateway
const byte WIFI_GATEWAY[4] = { 192, 168, 0, 1 };
// IP address for DNS server
const byte WIFI_SUBNET[4] = { 255, 255, 255, 0 };

// host name for DHCP
const String WIFI_HOSTNAME = "tempsensor";

// ---------------------------------------------------
// g_mqtt.ino constants

// host name
const String MQTT_HOST = "192.168.0.1";
// host port
const int MQTT_PORT = 1883;

// host username
const String MQTT_USERNAME = "mqttUsername";
// host password
const String MQTT_PASSWORD = "mqttPassword";

const String MQTT_ROOM = "mqttroom";

// ---------------------------------------------------
// k_ntp.ino constants

// NTP server name
const char* NTP_SERVER = "192.168.0.1";
// NTP time offset. Use 0 for UTC
const int NTP_TIME_OFFSET = 0;

// ---------------------------------------------------
// l_ftp.ino constants
const String FTP_USERNAME = "ftpUser";

const String FTP_PASSWORD = "ftpPassword";

// ---------------------------------------------------
// i_measure.ino constants

// value of the resistor(s) from ADC to the battery
const double BATTERY_RESISTOR_ADC_BATT_VALUE = 220000 + 470000;
// value of the resistor(s) from ADC to the ground
const double BATTERY_RESISTOR_ADC_GND_VALUE = 100000;
