#ifndef C_GLOBAL_STRUCTS_H_
#define C_GLOBAL_STRUCTS_H_

//#include <system.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <MQTTClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "LittleFS.h"
#include <NTPClient.h>
#include <TimeLib.h>

// ---------------------------------------------------
// global variables

// is the log file ready to write?
extern boolean global_log_enabled;

// ---------------------------------------------------
// wifi.cpp

// milliseconds until wifi connection was established
extern long global_wifiMillis;

// ---------------------------------------------------
// measure.cpp

//ADC_MODE(ADC_TOUT);

// DHT measurement
// static DHT global_dht(DHTPIN, DHTTYPE);
extern DHT global_dht;


// ---------------------------------------------------
// ntp.cpp
extern NTPClient global_timeClient;

// ---------------------------------------------------
// ftp.cpp

const boolean debug = true;

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

#endif /* C_GLOBAL_STRUCTS_H_ */