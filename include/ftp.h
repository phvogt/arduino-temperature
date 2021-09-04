#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "ftpconfig.h"

namespace arduino_temp {

// Transfer files via FTP
class FTP {
 private:
  // FTP connection configuration
  FTPConfig ftpConfig_;

  // FTP client
  WiFiClient ftpclient_;
  // FTP client for data port
  WiFiClient ftpdclient_;
  // output buffer
  char ftpoutBuf_[128];

 public:
  // Constructor
  // parameters:
  //   ftpConfig ... FTP connection configuration
  FTP(FTPConfig ftpConfig);

  // Uploads the file via FTP
  // parameters
  //   filename ... name of the file to upload
  // returns 0 in case of an error or 1 if ok
  byte ftpUploadFile(String filename);

  // handler for FTP errors
  // parameters: none
  // returns nothing
  void efail();

  // handler for received data
  // parameters: none
  // returns the received byte
  byte eRcv();
};

}  // namespace arduino_temp