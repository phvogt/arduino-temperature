#include "program.h"

#include <TimeLib.h>

#include "config.h"
#include "constants.h"

arduino_temp::Program::Program()
    : filehandler_(LOG_ENABLED),
      mqtt_(MQTT_HOST, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD,
            MQTT_MAX_RETRIES, MQTT_CONNECT_RETRY_DELAY_IN_MILLIS),
      wifi_(WIFI_SSID, WIFI_SSID_PASSWORD, WIFI_MAX_RETRIES,
            WIFI_CONNECT_RETRY_DELAY_IN_MILLIS, WIFI_RETRY_RESET_COUNT),
      ntp_(NTP_SERVER, NTP_TIME_OFFSET, NTP_UPDATE_INTERVAL),
      ftp_(FTP_SERVER_IP_ADDRESS, FTP_SERVER_PORT, FTP_DEBUG_ENABLED) {}

void arduino_temp::Program::setup() {
  // store the start time
  setStartMillis(millis());

  // setup serial connection
  Serial.begin(BAUDRATE);
  delay(10);

  Serial.println();
  Serial.println();

  // initially turn off wifi
  filehandler_.doLogInfoDoubleLine();
  filehandler_.doLogInfo("Turning Wifi off");
  wifi_.initWifiOff();

  filehandler_.doLogInfo("Starting with work mode: " + String(WORK_MODE));
  filehandler_.doLogInfoLine();

  if (WORK_MODE != WORK_MODE_NORMAL) {
    filehandler_.startFS();
  }

  boolean connected = false;

  switch (WORK_MODE) {
    case WORK_MODE_NORMAL:
      doWorkNormal();
      break;
    case WORK_MODE_LIST_DIRECTORY:
      filehandler_.listDirectory(LOGFILE_DIRECTORY);
      break;
    case WORK_MODE_PRINT_LOGFILES:
      filehandler_.printLogfiles(LOGFILE_NAME, LOGFILE_MAX_BACKUPS);
      break;
    case WORK_MODE_DELETE_LOGFILES:
      filehandler_.deleteLogfiles(LOGFILE_DIRECTORY, LOGFILE_NAME);
      break;
    case WORK_MODE_PRINT_FILE:
      filehandler_.printFileToSerial(PRINT_FILE_NAME);
      break;
    case WORK_MODE_FTPUPLOAD:
      connected = wifi_.connectWifi();
      if (connected) {
        ftp_.ftpUploadFile(LOGFILE_NAME);
      }
      break;
    default:
      filehandler_.doLogError("unknown work mode: " + String(WORK_MODE));
      filehandler_.doLogInfo("going to sleep");
      break;
  }

  if (WORK_MODE != WORK_MODE_NORMAL) {
    filehandler_.stopFS();
    coreFunctions_.sleepMaxTime();
  }
}

void arduino_temp::Program::loop() {
  // doLogWarn("doing nothing in loop!");
  coreFunctions_.sleepMaxTime();
}

void arduino_temp::Program::setStartMillis(unsigned long startMillis) {
  startMillis_ = startMillis;
}

void arduino_temp::Program::doWorkNormal() {
  filehandler_.doLogInfoDoubleLine();
  filehandler_.doLogInfo("Starting work at: " + String(millis()));

  timing_.timingStart();
  const String resetReason = coreFunctions_.determineResetReason();
  timing_.timingEnd("RST");

  measure_.initDHT();

  timing_.timingStart();
  struct measured_values_bat measuredValuesBatt =
      measure_.measureBatteryValues();
  timing_.timingEnd("BT");

  timing_.timingStart();
  struct measured_values_dht measuredValuesDht =
      measure_.measureDHTTempAndHumidity();
  timing_.timingEnd("DHT");

  timing_.timingStart();
  boolean connected = wifi_.connectWifi();
  timing_.timingEnd("WLAN");

  if (connected) {
    initNTP();
    timing_.timingStart();
    connected = mqtt_.setupMQTT();
    timing_.timingEnd("MQI");
  } else {
    wifi_.stopWifiForSleep();
    filehandler_.startFS();
    filehandler_.deleteFile(LOGFILE_NAME);
    dumpLog();
    filehandler_.setLogEnabled(true);
    renameLogLastBad();
    coreFunctions_.sleepMaxTime();
  }

  sendMQTTValues(resetReason, measuredValuesBatt, measuredValuesDht);
  sendMQTTDuration(startMillis_);

  long sleepTimeInMicros = calculateSleepTimeMicros(startMillis_);
  sendMQTTSleepTime(sleepTimeInMicros);
  sendMQTTTimings();
  delay(50);

  // don't log if everything went fine
  // startFS();
  // rotateLogs();
  // dumpLog();
  // global_log_enabled = true;
  // renameLogGood();
  // stopFS();

  wifi_.stopWifiForSleep();

  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("done, going to sleep");

  filehandler_.doLogInfoDoubleLine();

  coreFunctions_.doSleepForMicros(sleepTimeInMicros);
}

time_t arduino_temp::Program::initNTP() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("getting NTP");

  timing_.timingStart();
  filehandler_.doLogInfo("  calling initNTP");
  time_t currentEpoch = ntp_.initNTP();
  filehandler_.doLogInfo("  setting time. currentEpoch = " +
                         String(currentEpoch));
  setTime(currentEpoch);
  filehandler_.doLogInfoLine();
  timing_.timingEnd("NTP");

  return currentEpoch;
}

void arduino_temp::Program::sendMQTTValues(
    const String& resetReason,
    const struct measured_values_bat& measuredValuesBatt,
    const struct measured_values_dht& measuredValuesDht) {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("Sending data via MQTT for room: " + MQTT_ROOM);

  timing_.timingStart();
  // send all values
  // wifi connection time
  mqtt_.sendMqtt(MQTT_ROOM + "/wificonnection", String(wifi_.wifiMillis));
  // date time
  mqtt_.sendMqtt(MQTT_ROOM + "/datetime", ntp_.getDateTime());
  // reset reason
  mqtt_.sendMqtt(MQTT_ROOM + "/resetReason", resetReason);
  // battery
  mqtt_.sendMqtt(MQTT_ROOM + "/batt", String(measuredValuesBatt.battV));
  mqtt_.sendMqtt(MQTT_ROOM + "/battRaw", String(measuredValuesBatt.batt));
  // temperature / humidity values
  if (measuredValuesDht.couldReadValues) {
    mqtt_.sendMqtt(MQTT_ROOM + "/temperature", String(measuredValuesDht.temp));
    mqtt_.sendMqtt(MQTT_ROOM + "/humidity", String(measuredValuesDht.hum));
  }
  filehandler_.doLogInfoLine();
  timing_.timingEnd("MQV");
}

long arduino_temp::Program::sendMQTTDuration(unsigned long startMillis) {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("Calculating duration");

  timing_.timingStart();
  long wholeduration = millis() - startMillis;
  filehandler_.doLogInfo("duration: " + String(wholeduration) + " ms");
  mqtt_.sendMqtt(MQTT_ROOM + "/duration", String(wholeduration));
  filehandler_.doLogInfoLine();
  timing_.timingEnd("MQD");
  filehandler_.doLogInfoLine();
  filehandler_.doLogTime("all: " + String(wholeduration));

  return wholeduration;
}

void arduino_temp::Program::sendMQTTSleepTime(long sleepTimeInMicros) {
  timing_.timingStart();
  filehandler_.doLogInfo("sleeping for microseconds: " +
                         String(sleepTimeInMicros));
  mqtt_.sendMqtt(MQTT_ROOM + "/sleepTime", String(sleepTimeInMicros / 1000));
  filehandler_.doLogInfoLine();
  timing_.timingEnd("MQS");
}

void arduino_temp::Program::sendMQTTTimings() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("Sending timings");

  String timingsstr =
      timing_.getTimings() + ",A:" + String(millis() - startMillis_);
  timingsstr.replace(" ", "");
  mqtt_.sendMqtt(MQTT_ROOM + "/timings", timingsstr);
}

long arduino_temp::Program::calculateSleepTimeMicros(
    unsigned long startMillis) {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("Calculating sleep");

  // calculate sleeping time
  long sleepTimeInMicros = coreFunctions_.calcSleepTimeInMicroSeconds(
      CORE_SLEEP_TIME_IN_MILLIS, startMillis);
  return sleepTimeInMicros;
}

void arduino_temp::Program::dumpLog() {
  filehandler_.doLogInfoLine();

  filehandler_.doLogInfo("Dumping logfile");
  timing_.timingStart();
  // dump the current log buffer to the log file
  filehandler_.dumpLogBuffer();
  timing_.timingEnd("DUL");
}

void arduino_temp::Program::rotateLogs() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo(" Starting rotating files");

  timing_.timingStart();
  //	rotate log files
  filehandler_.rotateLogfiles(LOGFILE_NAME, LOGFILE_MAX_BACKUPS);
  filehandler_.doLogInfo("Done rotating files.");
  timing_.timingEnd("RLOG");
}

void arduino_temp::Program::renameLogGood() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("saving log file to " + LOGFILE_NAME +
                         LOGFILE_LASTGOOD_EXTENSION);

  timing_.timingStart();
  filehandler_.renameFile(LOGFILE_NAME,
                          LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION);
  timing_.timingEnd("LOG");
}

void arduino_temp::Program::renameLogLastBad() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("rename logfile with last bad extension");

  timing_.timingStart();
  filehandler_.doLogInfo("saving log file to " + LOGFILE_NAME +
                         LOGFILE_LASTBAD_EXTENSION);
  filehandler_.renameFile(LOGFILE_NAME,
                          LOGFILE_NAME + LOGFILE_LASTBAD_EXTENSION);
  timing_.timingEnd("RLOG");
}
