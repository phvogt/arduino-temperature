#ifndef ALL_HEADERS_H_
#define ALL_HEADERS_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <MQTTClient.h>
#include <DHT.h>
#include "LittleFS.h"
#include <NTPClient.h>
#include <TimeLib.h>

#include "constants.h"
#include "config.h"
#include "config_user.h"
#include "globals_structs.h"


// d_setup
void startFS();
void stopFS();
void doWorkNormal();
boolean connectWifi();

// f_core_functions
String getResetReason();
String getTimings();
void timingStart();
unsigned long timingEnd(String measuredName);
void sleepMaxTime();
long calcSleepTimeInMicroSeconds(int sleepTimeInMillis, int startTimeInMillis);
void doSleepForMicros(long sleepTimeInMicros);

// g_mqtt
boolean initMqtt(String host, const int port, String username, String password,
		int maxMqttConnectionRetries,
		unsigned long mqttConnectRetryDelayInMillis);
void sendMqtt(String topic, String message);

// h_wifi
int initWifi(String ssid, String password, int maxWifiConnectionRetries,
		unsigned long wifiConnectRetryDelayInMillis,
		int wifiConnectionRetryCountReset);
void initWifiConfig();
void initWifiOff();
void stopWifiForSleep();
boolean readWifiSettingsFromRTC();
void storeWifiSettingsInRTC();
uint32_t calculateCRC32(const uint8_t *data, size_t length);

// i_measure
struct measured_values_bat getBattery(double batteryFactor);
struct measured_values_dht getAndSendTemperatureAndHumidityData(
		boolean simulateValues, int dhtMaxReadValuesRetries,
		unsigned long dhtRetryDelayInMillis);

// j_file
void doLogInfo(String message);
void doLogWarn(String message);
void doLogError(String message);
void doLogTime(String message);
void writeLog(String logfileName, String logmessage);
void listDirectory(String directory);
void printLogfiles(String logfileName, int maxBackup);
void rotateLogfiles(String logfileName, int maxBackup);
void deleteLogfiles(String directory, String logfileName);
void dumpLogBuffer();
void printFileToSerial(String fileName);
void renameFile(String sourceFilename, String destFilename);
void deleteFile(String filename);

// k_ntp
time_t getNtpTime(int maxNtpRetries, unsigned long ntpRetryDelayInMillis);
String getDateTime();
String toStringLeadingZero(int value);

// l_ftp
byte ftpUploadFile(String filename);
byte eRcv();

#endif /* ALL_HEADERS_H_ */