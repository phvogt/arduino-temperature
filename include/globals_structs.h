#ifndef C_GLOBAL_STRUTS_H_
#define C_GLOBAL_STRUTS_H_

//#include <system.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <MQTTClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "FS.h"
#include <NTPClient.h>
#include <TimeLib.h>

// ---------------------------------------------------
// global variables

// is the log file ready to write?
//boolean global_log_enabled = false;
static boolean global_log_enabled = false;

// start time in milli seconds
//unsigned long global_startMillis = 0;
static unsigned long global_startMillis = 0;

// timestamp
//String global_timestamp = "<unkown time>";
static String global_timestamp = "<unkown time>";

// for measuring timing
//unsigned long global_timingStart = 0;
static unsigned long global_timingStart = 0;
// contains all the timings
//String global_timings = "";
static String global_timings = "";

// ---------------------------------------------------
// g_mqtt.ino

static WiFiClient global_mqttNet;
static MQTTClient global_mqttClient;

// ---------------------------------------------------
// h_wifi.ino

// milliseconds until wifi connection was established
static long global_wifiMillis = 0;

// from https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct {
  uint32_t crc32;   // 4 bytes
  uint8_t channel;  // 1 byte,   5 in total
  uint8_t bssid[6]; // 6 bytes, 11 in total
  uint8_t padding;  // 1 byte,  12 in total
} rtcData;

// ---------------------------------------------------
// i_measure.ino

//ADC_MODE(ADC_TOUT);

// DHT measurement
static DHT global_dht(DHTPIN, DHTTYPE);

// ---------------------------------------------------
// j_file.ino

// name of the logfile
static String global_logfileName = LOGFILE_NAME;

// buffer for logs until the log is ready
static String global_logBuffer = "";

// ---------------------------------------------------
// k_ntp.ino
static WiFiUDP global_ntpUDP;
static NTPClient global_timeClient(global_ntpUDP, NTP_SERVER, NTP_TIME_OFFSET,
                            NTP_UPDATE_INTERVAL);
static time_t global_time = 0;

// ---------------------------------------------------
// l_ftp.ino

static WiFiClient global_ftpclient;
static WiFiClient global_ftpdclient;
static char global_ftpoutBuf[128];
static char global_ftpoutCount;
static IPAddress global_ftpserver(192, 168, 2, 2);

static boolean debug = true;

// ---------------------------------------------------
// data structures

// measured values from DHT
 struct measured_values_dht {
  boolean couldReadValues;
  float temp;
  float hum;
};

// measured values from battery
 struct measured_values_bat {
  int batt;
  double battV;
};

#endif /* C_GLOBAL_STRUTS_H_ */