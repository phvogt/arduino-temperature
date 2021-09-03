#include "filehandler.h"

#include "LittleFS.h"
#include "ntp.h"

arduino_temp::FileHandler::FileHandler(boolean log_enabled) : ntp_() {
  log_enabled_ = log_enabled;
}

void arduino_temp::FileHandler::setLogEnabled(boolean log_enabled) {
  log_enabled_ = log_enabled;
}

void arduino_temp::FileHandler::startFS() {
  doLogInfoLine();
  doLogInfo("Starting LittleFS");

  // setup LittleFS to write files to flash file system
  LittleFS.begin();
}

void arduino_temp::FileHandler::stopFS() {
  doLogInfoLine();
  doLogInfo("Stopping LittleFS");

  setLogEnabled(false);
  LittleFS.end();
}

void arduino_temp::FileHandler::listDirectory(String directory) {
  Serial.println("all files in " + directory + " :");

  Dir dir = LittleFS.openDir(directory);
  while (dir.next()) {
    Serial.print(dir.fileName());
    Serial.print(" size: ");
    File f = dir.openFile("r");
    Serial.println(f.size());
  }
}

String arduino_temp::FileHandler::getLogfileBackupName(String logfileName,
                                                       int num) {
  return logfileName + "." + String(num);
}

// Rotate the log files.
// parameters:
//   logfileName ... base name for log files
//   maxBackup ... maximum backups
// returns nothing
void arduino_temp::FileHandler::rotateLogfiles(String logfileName,
                                               int maxBackup) {
  Serial.println("rotating log files: " + logfileName);

  // remove last file
  String lastBackupName = getLogfileBackupName(logfileName, maxBackup);
  if (LittleFS.exists(lastBackupName)) {
    Serial.println("  removing: " + lastBackupName);
    LittleFS.remove(lastBackupName);
  }
  // rotate all backup files
  for (int i = maxBackup - 1; i >= 0; i--) {
    String currentBackupName = getLogfileBackupName(logfileName, i);
    if (LittleFS.exists(currentBackupName)) {
      String nextBackupName = getLogfileBackupName(logfileName, i + 1);
      Serial.println("  renaming backup: " + currentBackupName +
                     " to: " + nextBackupName);
      LittleFS.rename(currentBackupName, nextBackupName);
    }
  }
  // rotate current logfile to first backup file
  if (LittleFS.exists(logfileName)) {
    String firstBackupName = getLogfileBackupName(logfileName, 0);
    Serial.println("  renaming current: " + logfileName +
                   " to: " + firstBackupName);
    LittleFS.rename(logfileName, firstBackupName);
  }
}

void arduino_temp::FileHandler::printLogfiles(String logfileName,
                                              int maxBackup) {
  Serial.println("printing all log files for: " + logfileName);

  // print current logfile
  if (LittleFS.exists(logfileName)) {
    printFileToSerial(logfileName);
  }

  // print last good logfile
  if (LittleFS.exists(logfileName + LOGFILE_LASTGOOD_EXTENSION)) {
    printFileToSerial(logfileName + LOGFILE_LASTGOOD_EXTENSION);
  }

  // print last bad logfile
  if (LittleFS.exists(logfileName + LOGFILE_LASTBAD_EXTENSION)) {
    printFileToSerial(logfileName + LOGFILE_LASTBAD_EXTENSION);
  }

  // print all backup files
  for (int i = 0; i <= maxBackup; i++) {
    String currentBackupName = getLogfileBackupName(logfileName, i);
    if (LittleFS.exists(currentBackupName)) {
      printFileToSerial(currentBackupName);
    }
  }
}

void arduino_temp::FileHandler::deleteLogfiles(String directory,
                                               String logfileName) {
  Serial.println("deleting in " + directory + " the log files: " + logfileName);

  Dir dir = LittleFS.openDir(directory);
  while (dir.next()) {
    String filename = dir.fileName();
    if (filename.indexOf(logfileName) > -1) {
      Serial.println("  deleting: " + filename);
      LittleFS.remove(filename);
    }
  }
}

void arduino_temp::FileHandler::doLog(String logLevel, String message) {
  String logLine = ntp_.getDateTime() + " " + String(millis()) + " " +
                   logLevel + ": " + message;
  Serial.println(logLine);

  if (log_enabled_) {
    if (logBuffer_ == "") {
      logBuffer_ += logLine;
    } else {
      logBuffer_ += "\n" + logLine;
    }
  }
}

void arduino_temp::FileHandler::doLogInfo(String message) {
  doLog(LOGLEVEL_INFO, message);
}

void arduino_temp::FileHandler::doLogWarn(String message) {
  doLog(LOGLEVEL_WARN, message);
}

void arduino_temp::FileHandler::doLogError(String message) {
  doLog(LOGLEVEL_ERROR, message);
}

void arduino_temp::FileHandler::doLogTime(String message) {
  doLog(LOGLEVEL_TIME, message);
}

void arduino_temp::FileHandler::doLogInfoLine() {
  doLogInfo("------------------------------------------------------");
}

void arduino_temp::FileHandler::doLogInfoDoubleLine() {
  doLogInfo("======================================================");
}

void arduino_temp::FileHandler::dumpLogBuffer(String logfileName) {
  writeLog(logfileName, logBuffer_);
}

void arduino_temp::FileHandler::writeLog(String logfileName,
                                         String logmessage) {
  // open file for appending new blank line to EOF.
  File f = LittleFS.open(logfileName, "w");

  f.print(logmessage);
  f.print('\n');
  f.flush();
  delay(1);
  f.close();
}

void arduino_temp::FileHandler::printFileToSerial(String fileName) {
  Serial.println(
      "************************************************************************"
      "************************************");

  File f = LittleFS.open(fileName, "r");
  if (!f) {
    Serial.println("file \"" + fileName + "\" open failed");
    Serial.println(
        "**********************************************************************"
        "**************************************");
    return;
  }

  Serial.println("Content of file \"" + fileName + "\":");
  while (f.available()) {
    // Lets read line by line from the file
    String line = f.readStringUntil('\n');
    Serial.println(line);
  }
  f.close();
  Serial.println(
      "************************************************************************"
      "************************************");
}

void arduino_temp::FileHandler::copyFile(String sourceFilename,
                                         String destFilename) {
  if (!LittleFS.exists(sourceFilename)) {
    Serial.println("could not find \"" + sourceFilename + "\"");
    return;
  }
  File fSrc = LittleFS.open(sourceFilename, "r");
  if (!fSrc) {
    Serial.println("error opening \"" + sourceFilename + "\" for reading");
    return;
  }
  File fDst = LittleFS.open(destFilename, "w");
  if (!fDst) {
    Serial.println("error opening \"" + destFilename + "\" for writing");
    return;
  }
  while (fSrc.available()) {
    fDst.print(fSrc.readStringUntil('\n') + "\n");
  }
  fDst.close();
  fSrc.close();
}

void arduino_temp::FileHandler::renameFile(String sourceFilename,
                                           String destFilename) {
  if (LittleFS.exists(destFilename)) {
    LittleFS.remove(destFilename);
  }

  LittleFS.rename(sourceFilename, destFilename);
}

void arduino_temp::FileHandler::deleteFile(String filename) {
  if (LittleFS.exists(filename)) {
    LittleFS.remove(filename);
  }
}
