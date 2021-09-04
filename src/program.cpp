#include "program.h"

#include <TimeLib.h>

#include "config.h"
#include "constants.h"
#include "filehandler.h"

arduino_temp::Config config;

arduino_temp::Program::Program()
    : ntp_(config.NTP_CONFIG),
      mqtt_(config.MQTT_CONFIG),
      wifi_(config.WIFI_CONFIG),
      coreFunctions_(config.COREFUNCTIONS_CONFIG),
      measure_(config.MEASURE_CONFIG),
      ftp_(config.FTP_CONFIG) {}

void arduino_temp::Program::setup() {
  // store the start time
  setStartMillis(millis());

  // setup serial connection
  Serial.begin(BAUDRATE);
  delay(10);

  Serial.println();
  Serial.println();

  // initially turn off wifi
  FileHandler::getInstance().doLogInfoDoubleLine();
  FileHandler::getInstance().doLogInfo("Turning Wifi off");
  wifi_.initWifiOff();

  FileHandler::getInstance().doLogInfo("Starting with work mode: " +
                                       String(WORK_MODE));
  FileHandler::getInstance().doLogInfoLine();

  boolean connected = false;

  switch (WORK_MODE) {
    case WORK_MODE_NORMAL:
      if (LOG_ENABLED) {
        FileHandler::getInstance().startFS();
      }
      doWorkNormal();
      if (LOG_ENABLED) {
        FileHandler::getInstance().stopFS();
      }
      break;
    case WORK_MODE_LIST_DIRECTORY:
      FileHandler::getInstance().startFS();
      FileHandler::getInstance().listDirectory(LOGFILE_DIRECTORY);
      FileHandler::getInstance().stopFS();
      break;
    case WORK_MODE_PRINT_LOGFILES:
      FileHandler::getInstance().startFS();
      FileHandler::getInstance().printLogfiles(LOGFILE_DIRECTORY + LOGFILE_NAME,
                                               LOGFILE_MAX_BACKUPS);
      FileHandler::getInstance().stopFS();
      break;
    case WORK_MODE_DELETE_LOGFILES:
      FileHandler::getInstance().startFS();
      FileHandler::getInstance().deleteLogfiles(LOGFILE_DIRECTORY,
                                                LOGFILE_NAME);
      FileHandler::getInstance().stopFS();
      break;
    case WORK_MODE_PRINT_FILE:
      FileHandler::getInstance().startFS();
      FileHandler::getInstance().printFileToSerial(PRINT_FILE_NAME);
      FileHandler::getInstance().stopFS();
      break;
    case WORK_MODE_FTPUPLOAD:
      FileHandler::getInstance().startFS();
      connected = wifi_.connectWifi();
      if (connected) {
        ftp_.ftpUploadFile(LOGFILE_DIRECTORY + LOGFILE_NAME);
      }
      FileHandler::getInstance().stopFS();
      break;
    default:
      FileHandler::getInstance().doLogError("unknown work mode: " +
                                            String(WORK_MODE));
      FileHandler::getInstance().doLogInfo("going to sleep");
      break;
  }

  coreFunctions_.doSleepForMicros(CORE_SLEEP_TIME_IN_MICROS);
}

void arduino_temp::Program::loop() {
  // doLogWarn("doing nothing in loop!");
  coreFunctions_.doSleepForMicros(CORE_SLEEP_TIME_IN_MICROS);
}

void arduino_temp::Program::setStartMillis(unsigned long startMillis) {
  startMillis_ = startMillis;
}

void arduino_temp::Program::doWorkNormal() {
  FileHandler::getInstance().doLogInfoDoubleLine();
  FileHandler::getInstance().doLogInfo("Starting work at: " + String(millis()));

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
    if (LOG_ENABLED) {
      rotateLogs(LOGFILE_DIRECTORY + LOGFILE_NAME);
      dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME);
      dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME + LOGFILE_LASTBAD_EXTENSION);
    }
    coreFunctions_.doSleepForMicros(CORE_SLEEP_TIME_IN_MICROS);
  }

  sendMQTTValues(resetReason, measuredValuesBatt, measuredValuesDht);
  sendMQTTDuration(startMillis_);

  long sleepTimeInMicros = calculateSleepTimeMicros(startMillis_);
  sendMQTTSleepTime(sleepTimeInMicros);
  sendMQTTTimings();
  delay(50);

  wifi_.stopWifiForSleep();

  FileHandler::getInstance().doLogInfoLine();

  if (LOG_ENABLED) {
    rotateLogs(LOGFILE_DIRECTORY + LOGFILE_NAME);
    dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME);
    dumpLog(LOGFILE_DIRECTORY + LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION);
    FileHandler::getInstance().stopFS();
    FileHandler::getInstance().doLogInfoLine();
  }

  FileHandler::getInstance().doLogInfo("done, going to sleep");

  FileHandler::getInstance().doLogInfoDoubleLine();

  coreFunctions_.doSleepForMicros(sleepTimeInMicros);
}

time_t arduino_temp::Program::initNTP() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("getting NTP");

  timing_.timingStart();
  FileHandler::getInstance().doLogInfo("  calling initNTP");
  time_t currentEpoch = ntp_.initNTP();
  FileHandler::getInstance().doLogInfo("  setting time. currentEpoch = " +
                                       String(currentEpoch));
  setTime(currentEpoch);
  FileHandler::getInstance().doLogInfoLine();
  timing_.timingEnd("NTP");

  return currentEpoch;
}

void arduino_temp::Program::sendMQTTValues(
    const String& resetReason,
    const struct measured_values_bat& measuredValuesBatt,
    const struct measured_values_dht& measuredValuesDht) {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Sending data via MQTT for room: " +
                                       mqtt_.getTopic());

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
  FileHandler::getInstance().doLogInfoLine();
  timing_.timingEnd("MQV");
}

long arduino_temp::Program::sendMQTTDuration(unsigned long startMillis) {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Calculating duration");

  timing_.timingStart();
  long wholeduration = millis() - startMillis;
  FileHandler::getInstance().doLogInfo("duration: " + String(wholeduration) +
                                       " ms");
  mqtt_.sendMqtt(mqtt_.getTopic() + "/duration", String(wholeduration));
  FileHandler::getInstance().doLogInfoLine();
  timing_.timingEnd("MQD");
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogTime("all: " + String(wholeduration));

  return wholeduration;
}

void arduino_temp::Program::sendMQTTSleepTime(long sleepTimeInMicros) {
  timing_.timingStart();
  FileHandler::getInstance().doLogInfo("sleeping for microseconds: " +
                                       String(sleepTimeInMicros));
  mqtt_.sendMqtt(mqtt_.getTopic() + "/sleepTime",
                 String(sleepTimeInMicros / 1000));
  FileHandler::getInstance().doLogInfoLine();
  timing_.timingEnd("MQS");
}

void arduino_temp::Program::sendMQTTTimings() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Sending timings");

  String timingsstr =
      timing_.getTimings() + ",A:" + String(millis() - startMillis_);
  timingsstr.replace(" ", "");
  mqtt_.sendMqtt(mqtt_.getTopic() + "/timings", timingsstr);
}

long arduino_temp::Program::calculateSleepTimeMicros(
    unsigned long startMillis) {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Calculating sleep");

  // calculate sleeping time
  long sleepTimeInMicros = coreFunctions_.calcSleepTimeInMicroSeconds(
      CORE_SLEEP_TIME_IN_MICROS, startMillis);
  return sleepTimeInMicros;
}

void arduino_temp::Program::dumpLog(String logfileName) {
  FileHandler::getInstance().doLogInfoLine();

  FileHandler::getInstance().doLogInfo("Dumping logfile: " + logfileName);
  timing_.timingStart();
  // dump the current log buffer to the log file
  FileHandler::getInstance().dumpLogBuffer(logfileName);
  timing_.timingEnd("DUL");
}

void arduino_temp::Program::rotateLogs(String logfileName) {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo(" Starting rotating files: " +
                                       logfileName);

  timing_.timingStart();
  //	rotate log files
  FileHandler::getInstance().rotateLogfiles(logfileName, LOGFILE_MAX_BACKUPS);
  FileHandler::getInstance().doLogInfo("Done rotating files.");
  timing_.timingEnd("RLOG");
}
