#ifndef CONSTANTS_H_
#define CONSTANTS_H_

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
const int WORK_MODE = WORK_MODE_NORMAL;

// ---------------------------------------------------
// c_global_structs.ino constants

// constants for DHT

// type of DHT
#define DHTTYPE DHT22
// pin the DHT is connected
#define DHTPIN 12

// constants for communication
#define BAUDRATE 115200
//#define BAUDRATE 74880

// ---------------------------------------------------
// g_mqtt.ino constants

// only try this number of times to connect to MQTT (with delay below)
const int MQTT_MAX_RETRIES = 3;
// delay between MQTT connection retries
const unsigned long MQTT_CONNECT_RETRY_DELAY_IN_MILLIS = 500;

// ---------------------------------------------------
// h_wifi.ino constants

// only try this number times to connect to wifi (with delay below)
static int WIFI_MAX_RETRIES = 200;
// delay between wifi connection retries
static unsigned long WIFI_CONNECT_RETRY_DELAY_IN_MILLIS = 100;
// after this number of retries the WiFi gets disconnected and reconnected
static int WIFI_RETRY_RESET_COUNT = 50;

// ---------------------------------------------------
// i_measure.ino constants

// maximum times to try to read values from DHT (with delay below)
const int DHT_MAX_READ_VALUES_RETRIES = 10;
// delay between wifi connection retries
static unsigned long DHT_DELAY_IN_MILLIS = 200;

// simulate values for debugging purposes
const boolean DHT_SIMULATE_VALUES = false;

// ---------------------------------------------------
// i_file.ino constants

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
const String PRINT_FILE_NAME = LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION;

// log levels
const String LOGLEVEL_INFO = "INFO ";
const String LOGLEVEL_WARN = "WARN ";
const String LOGLEVEL_ERROR = "ERROR";
const String LOGLEVEL_TIME = "TIME ";

// ---------------------------------------------------
// j_ntp.ino constants

// NTP update interval
const int NTP_UPDATE_INTERVAL = 60000;

// only try this number times to get valid time (with delay below)
const int NTP_MAX_RETRIES = 2;
// delay between ntp retries
const unsigned long NTP_RETRY_DELAY_IN_MILLIS = 100;

#endif /* CONSTANTS_H_ */