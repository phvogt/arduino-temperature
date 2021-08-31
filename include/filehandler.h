#pragma once

#include <Arduino.h>

#include "constants.h"
#include "ntp.h"

namespace arduino_temp {

// Handling of files / logging
class FileHandler {
 private:
  // flag if the log should be enabled
  boolean log_enabled_ = LOG_ENABLED;
  // name of the logfile
  String logfileName_ = LOGFILE_NAME;
  // buffer for logs until the log is ready
  String logBuffer_ = "";

  // NTP to get time for logging
  NTP ntp_;

 public:
  FileHandler(boolean log_enabled = LOG_ENABLED);

  // Sets if the log should be enabled (true) or not (false)
  // parameters:
  //   log_enabled ... whether the log should be enabled (true) or not (false)
  void setLogEnabled(boolean log_enabled);

  // Start the file system and enable logging.
  // parameters: none
  // returns nothing
  void startFS();

  // Disables logging to file system and stops the file system.
  // parameters: none
  // returns nothing
  void stopFS();

  // List the content of the directory.
  // parameters:
  //   directory ... the directory to list
  // returns nothing
  void listDirectory(String directory);

  // Get the name for the log file backup.
  // parameters:
  //   logfileName ... base name of the log files
  //   num ... number of the log file
  // returns the name of the log file
  String getLogfileBackupName(String logfileName, int num);

  // Rotate the log files.
  // parameters:
  //   logfileName ... base name for log files
  //   maxBackup ... maximum backups
  // returns nothing
  void rotateLogfiles(String logfileName, int maxBackup);

  // Prints all log files.
  // parameters:
  //   logfileName ... base name for log files
  // maxBackup ... maximum backups
  // returns nothing
  void printLogfiles(String logfileName, int maxBackup);

  // Deletes all log files.
  // parameters:
  //   directory ... the directory to delete the logfiles in
  //   logfileName ... base name for log files
  // returns nothing
  void deleteLogfiles(String directory, String logfileName);

  // Writes message with logLevel to serial and log file if global_log_enabled
  // is true. If global_log_enabled is false, the content is written to an
  // internal log buffer. Use dumpLogBuffer() to write the content to the log
  // file. parameters:
  //   logLevel ... log level
  //   message ... message to log
  // returns nothing
  void doLog(String logLevel, String message);

  // Logs the message with LOGLEVEL_INFO.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void doLogInfo(String message);

  // Logs the message with LOGLEVEL_WARN.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void doLogWarn(String message);

  // Logs the message with LOGLEVEL_ERROR.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void doLogError(String message);

  // Logs the message with LOGLEVEL_TIME.
  // parameters:
  //   message ... the message to log
  // returns nothing
  void doLogTime(String message);

  // Logs a line LOGLEVEL_INFO.
  void doLogInfoLine();

  // Logs a double line LOGLEVEL_INFO.
  void doLogInfoDoubleLine();

  // Dumps the content of the log buffer to the log file.
  // parameters: none
  // returns nothing
  void dumpLogBuffer();

  // Write to log file.
  // parameters:
  //   logfileName ... base name of the log file
  //   logmessage ... message to log
  // returns nothing
  void writeLog(String logfileName, String logmessage);

  // Print the contents of the file to the serial.
  // parameters:
  //   fileName ... name of the log file
  // returns nothing
  void printFileToSerial(String fileName);

  // Copies the source file to the destination file. Overwrites the destination
  // file if it already! parameters:
  //   sourceFilename ... name of the source file
  //   destFilename ... name of the destination file
  // returns nothing
  void copyFile(String sourceFilename, String destFilename);

  // Renames the file. Overwrites the destination file if it already exists!
  // parameters:
  //   sourceFilename ... name of the source file
  //   destFilename ... name of the destination file
  // returns nothing
  void renameFile(String sourceFilename, String destFilename);

  // Deletes the file if it exists.
  // parameters:
  //   filename ... name of the file
  // returns nothing
  void deleteFile(String filename);
};

}  // namespace arduino_temp