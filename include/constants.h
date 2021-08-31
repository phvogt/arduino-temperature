#pragma once

#include <Arduino.h>
#include <DHT.h>
// contains constants

// ---------------------------------------------------
// all modes of work

// normal mode with measuring and sending results
const int WORK_MODE_NORMAL = 0;
// debug mode: list directory
const int WORK_MODE_LIST_DIRECTORY = 1;
// debug mode: print the log files
const int WORK_MODE_PRINT_LOGFILES = 2;
// debug mode: delete all log files!
const int WORK_MODE_DELETE_LOGFILES = 3;
// debug mode: print the file
const int WORK_MODE_PRINT_FILE = 4;
// debug mode: upload the log file
const int WORK_MODE_FTPUPLOAD = 5;

// what the program should do
// const int WORK_MODE = WORK_MODE_NORMAL;
const int WORK_MODE = WORK_MODE_NORMAL;

// constants for communication
#define BAUDRATE 115200
//#define BAUDRATE 74880

// ---------------------------------------------------
// MQTT

// only try this number of times to connect to MQTT (with delay below)
const int MQTT_MAX_RETRIES = 3;
// delay between MQTT connection retries
const unsigned long MQTT_CONNECT_RETRY_DELAY_IN_MILLIS = 500;

// ---------------------------------------------------
// WIFI

// only try this number times to connect to wifi (with delay below)
const int WIFI_MAX_RETRIES = 200;
// delay between wifi connection retries
const unsigned long WIFI_CONNECT_RETRY_DELAY_IN_MILLIS = 100;
// after this number of retries the WiFi gets disconnected and reconnected
const int WIFI_RETRY_RESET_COUNT = 50;

// ---------------------------------------------------
// Measuring

// type of DHT
const uint8_t DHTTYPE = DHT22;
// pin the DHT is connected
const uint8_t DHTPIN = 12;

// maximum times to try to read values from DHT (with delay below)
const int DHT_MAX_READ_VALUES_RETRIES = 10;
// delay between wifi connection retries
const unsigned long DHT_DELAY_IN_MILLIS = 1100;

// simulate values for debugging purposes
const boolean DHT_SIMULATE_VALUES = false;

// ---------------------------------------------------
// Filehandler

const boolean LOG_ENABLED = true;

const String LOGFILE_DIRECTORY = "/";

// name of the logfile to use
const String LOGFILE_NAME = "/temperature.log";

// file name extension of the last good run
const String LOGFILE_LASTGOOD_EXTENSION = ".lastgood";

// file name extension of the last bad run
const String LOGFILE_LASTBAD_EXTENSION = ".lastbad";

// maximum number of backups for log file
const int LOGFILE_MAX_BACKUPS = 2;

// name of the file to print
// const String PRINT_FILE_NAME = LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION;
const String PRINT_FILE_NAME = LOGFILE_NAME;

// log levels
const String LOGLEVEL_INFO = "INFO ";
const String LOGLEVEL_WARN = "WARN ";
const String LOGLEVEL_ERROR = "ERROR";
const String LOGLEVEL_TIME = "TIME ";

// ---------------------------------------------------
// NTP

// NTP update interval
const int NTP_UPDATE_INTERVAL = 60000;

// only try this number times to get valid time (with delay below)
const int NTP_MAX_RETRIES = 2;
// delay between ntp retries
const unsigned long NTP_RETRY_DELAY_IN_MILLIS = 100;

// ---------------------------------------------------
// FTP

const boolean FTP_DEBUG_ENABLED = false;

