#include <Arduino.h>

// project for measuring the temperature with a ESP 12e and a DHT22 sensor

// see https://www.vogt.or.at/hardware/temperaturesensor
// see https://github.com/phvogt/arduino-temperature

// headers:
// constants       ... contains the constants for the project
// config          ... contains the configuration for the temperature sensor
// globals_struct  ... global variables / structs
// all_headers     ... combines all header files

// cpp:
// setup           ... the setup function
// loop            ... the loop function
// core_functions  ... all core functions
// mqtt            ... MQTT functions
// wifi            ... Wifi functions
// measure         ... functions for measuring
// file            ... functions for (log-) file handling
// ntp             ... functions for ntp
// ftp             ... functions for ftp

// used libraries:
//
// name=ESP8266WiFi
// version=1.0
// author=Ivan Grokhotkov
// category=Communication
// url=
//
// name=MQTT
// version=2.4.3
// author=Joel Gaehwiler <joel.gaehwiler@gmail.com>
// category=Communication
// url=https://github.com/256dpi/arduino-mqtt
//
// name=DHT sensor library
// version=1.3.7
// author=Adafruit
// category=Sensors
// url=https://github.com/adafruit/DHT-sensor-library
//
// needed for DHT
// name=Adafruit Unified Sensor
// version=1.0.3
// author=Adafruit <info@adafruit.com>
// category=Sensors
// url=https://github.com/adafruit/Adafruit_Sensor
//
//
// name=NTPClient
// version=3.1.0
// author=Fabrice Weinberg
// category=Timing
// url=https://github.com/arduino-libraries/NTPClient
//
// name=Time
// version=1.5
// author=Michael Margolis
// category=Timing
// url=http://playground.arduino.cc/code/time
