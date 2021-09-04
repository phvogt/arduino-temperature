#include "program.h"

#include <TimeLib.h>

#include "config.h"
#include "constants.h"

arduino_temp::Program::Program()
    : ntp_(config_.NTP_CONFIG),
      logger_(ntp_, config_.logEnabled_),
      fileHandler_(logger_),
      timing_(logger_),
      mqtt_(config_.MQTT_CONFIG, logger_),
      wifi_(config_.WIFI_CONFIG, logger_),
      coreFunctions_(config_.COREFUNCTIONS_CONFIG, logger_),
      measure_(config_.MEASURE_CONFIG, logger_),
      ftp_(config_.FTP_CONFIG) {}

void arduino_temp::Program::setup() {
  // store the start time
  setStartMillis(millis());

  // setup serial connection
  Serial.begin(BAUDRATE);
  delay(10);

  Serial.println();
  Serial.println();

  // initially turn off wifi
  logger_.logInfoDoubleLine();
  logger_.logInfo("Turning Wifi off");
  wifi_.initWifiOff();

  logger_.logInfo("Starting with work mode: " + String(config_.workMode_));
  logger_.logInfoLine();

  boolean connected = false;

  switch (config_.workMode_) {
    case WORK_MODE_NORMAL:
      if (config_.logEnabled_) {
        fileHandler_.startFS();
      }
      doWorkNormal();
      if (config_.logEnabled_) {
        fileHandler_.stopFS();
      }
      break;
    case WORK_MODE_LIST_DIRECTORY:
      fileHandler_.startFS();
      fileHandler_.listDirectory(LOGFILE_DIRECTORY);
      fileHandler_.stopFS();
      break;
    case WORK_MODE_PRINT_LOGFILES:
      fileHandler_.startFS();
      fileHandler_.printLogfiles(LOGFILE_DIRECTORY + LOGFILE_NAME,
                                 LOGFILE_MAX_BACKUPS);
      fileHandler_.stopFS();
      break;
    case WORK_MODE_DELETE_LOGFILES:
      fileHandler_.startFS();
      fileHandler_.deleteLogfiles(LOGFILE_DIRECTORY, LOGFILE_NAME);
      fileHandler_.stopFS();
      break;
    case WORK_MODE_PRINT_FILE:
      fileHandler_.startFS();
      fileHandler_.printFileToSerial(PRINT_FILE_NAME);
      fileHandler_.stopFS();
      break;
    case WORK_MODE_FTPUPLOAD:
      fileHandler_.startFS();
      connected = wifi_.connectWifi();
      if (connected) {
        ftp_.ftpUploadFile(LOGFILE_DIRECTORY + LOGFILE_NAME);
      }
      fileHandler_.stopFS();
      break;
    default:
      logger_.logError("unknown work mode: " + String(config_.workMode_));
      logger_.logInfo("going to sleep");
      break;
  }

  coreFunctions_.doSleepForMicros(config_.sleepTimeBetweenMeasureInMicros);
}

void arduino_temp::Program::loop() {
  // logWarn("doing nothing in loop!");
  coreFunctions_.doSleepForMicros(config_.sleepTimeBetweenMeasureInMicros);
}

void arduino_temp::Program::setStartMillis(unsigned long startMillis) {
  startMillis_ = startMillis;
}

void arduino_temp::Program::doWorkNormal() {
  logger_.logInfoDoubleLine();
  logger_.logInfo("Starting work at: " + String(millis()));

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
    if (config_.logEnabled_) {
      rotateLogs(LOGFILE_DIRECTORY + LOGFILE_NAME);
      dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME);
      dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME + LOGFILE_LASTBAD_EXTENSION);
    }
    coreFunctions_.doSleepForMicros(config_.sleepTimeBetweenMeasureInMicros);
  }

  sendMQTTValues(resetReason, measuredValuesBatt, measuredValuesDht);
  sendMQTTDuration(startMillis_);

  long sleepTimeInMicros = calculateSleepTimeMicros(startMillis_);
  sendMQTTSleepTime(sleepTimeInMicros);
  sendMQTTTimings();
  delay(50);

  wifi_.stopWifiForSleep();

  logger_.logInfoLine();

  if (config_.logEnabled_) {
    rotateLogs(LOGFILE_DIRECTORY + LOGFILE_NAME);
    dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME);
    dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION);
    fileHandler_.stopFS();
    logger_.logInfoLine();
  }

  logger_.logInfo("done, going to sleep");

  logger_.logInfoDoubleLine();

  coreFunctions_.doSleepForMicros(sleepTimeInMicros);
}

time_t arduino_temp::Program::initNTP() {
  logger_.logInfoLine();
  logger_.logInfo("getting NTP");

  timing_.timingStart();
  logger_.logInfo("  calling initNTP");
  time_t currentEpoch = ntp_.initNTP();
  logger_.logInfo("  setting time. currentEpoch = " + String(currentEpoch));
  setTime(currentEpoch);
  logger_.logInfoLine();
  timing_.timingEnd("NTP");

  return currentEpoch;
}

void arduino_temp::Program::sendMQTTValues(
    const String& resetReason,
    const struct measured_values_bat& measuredValuesBatt,
    const struct measured_values_dht& measuredValuesDht) {
  logger_.logInfoLine();
  logger_.logInfo("Sending data via MQTT for room: " + mqtt_.getTopic());

  timing_.timingStart();
  // send all values
  // wifi connection time
  mqtt_.sendMqtt(mqtt_.getTopic() + "/wificonnection",
                 String(wifi_.wifiMillis));
  // date time
  mqtt_.sendMqtt(mqtt_.getTopic() + "/datetime", ntp_.getDateTime());
  // reset reason
  mqtt_.sendMqtt(mqtt_.getTopic() + "/resetReason", resetReason);
  // battery
  mqtt_.sendMqtt(mqtt_.getTopic() + "/batt", String(measuredValuesBatt.battV));
  mqtt_.sendMqtt(mqtt_.getTopic() + "/battRaw",
                 String(measuredValuesBatt.batt));
  // temperature / humidity values
  if (measuredValuesDht.couldReadValues) {
    mqtt_.sendMqtt(mqtt_.getTopic() + "/temperature",
                   String(measuredValuesDht.temp));
    mqtt_.sendMqtt(mqtt_.getTopic() + "/humidity",
                   String(measuredValuesDht.hum));
  }
  logger_.logInfoLine();
  timing_.timingEnd("MQV");
}

long arduino_temp::Program::sendMQTTDuration(unsigned long startMillis) {
  logger_.logInfoLine();
  logger_.logInfo("Calculating duration");

  timing_.timingStart();
  long wholeduration = millis() - startMillis;
  logger_.logInfo("duration: " + String(wholeduration) + " ms");
  mqtt_.sendMqtt(mqtt_.getTopic() + "/duration", String(wholeduration));
  logger_.logInfoLine();
  timing_.timingEnd("MQD");
  logger_.logInfoLine();
  logger_.logTime("all: " + String(wholeduration));

  return wholeduration;
}

void arduino_temp::Program::sendMQTTSleepTime(long sleepTimeInMicros) {
  timing_.timingStart();
  logger_.logInfo("sleeping for microseconds: " + String(sleepTimeInMicros));
  mqtt_.sendMqtt(mqtt_.getTopic() + "/sleepTime",
                 String(sleepTimeInMicros / 1000));
  logger_.logInfoLine();
  timing_.timingEnd("MQS");
}

void arduino_temp::Program::sendMQTTTimings() {
  logger_.logInfoLine();
  logger_.logInfo("Sending timings");

  String timingsstr =
      timing_.getTimings() + ",A:" + String(millis() - startMillis_);
  timingsstr.replace(" ", "");
  mqtt_.sendMqtt(mqtt_.getTopic() + "/timings", timingsstr);
}

long arduino_temp::Program::calculateSleepTimeMicros(
    unsigned long startMillis) {
  logger_.logInfoLine();
  logger_.logInfo("Calculating sleep");

  // calculate sleeping time
  long sleepTimeInMicros = coreFunctions_.calcSleepTimeInMicroSeconds(
      config_.sleepTimeBetweenMeasureInMicros, startMillis * 1000);
  return sleepTimeInMicros;
}

void arduino_temp::Program::dumpLog(String logfileName) {
  logger_.logInfoLine();

  logger_.logInfo("Dumping logfile: " + logfileName);
  timing_.timingStart();
  // dump the current log buffer to the log file
  fileHandler_.writeFile(logfileName, logger_.getLogBuffer());
  timing_.timingEnd("DUL");
}

void arduino_temp::Program::rotateLogs(String logfileName) {
  logger_.logInfoLine();
  logger_.logInfo(" Starting rotating files: " + logfileName);

  timing_.timingStart();
  //	rotate log files
  fileHandler_.rotateLogfiles(logfileName, LOGFILE_MAX_BACKUPS);
  logger_.logInfo("Done rotating files.");
  timing_.timingEnd("RLOG");
}
