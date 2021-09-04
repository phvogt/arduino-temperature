#pragma once

#include <Arduino.h>

#include "logger.h"

namespace arduino_temp {

// Handling of files.
class FileHandler {
 private:
  // logger
  Logger logger_;

  // Get the name for the log file backup.
  // parameters:
  //   logfileName ... base name of the log files
  //   num ... number of the log file
  // returns the name of the log file
  String getLogfileBackupName(String logfileName, int num);

 public:
  // Constructor.
  //   logger ... Logger
  FileHandler(Logger logger);

  // Start the file system and enable logging.
  // parameters: none
  // returns nothing
  void startFS();

  // Disables logging to file system and stops the file system.
  // parameters: none
  // returns nothing
  void stopFS();

  // Write to  file.
  // parameters:
  //   fileName ... filename to write to
  //   content ... content to write
  // returns nothing
  void writeFile(String fileName, String content);

  // List the content of the directory.
  // parameters:
  //   directory ... the directory to list
  // returns nothing
  void listDirectory(String directory);

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