#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "constants.h"
#include "filehandler.h"

namespace arduino_temp {

// Transfer files via FTP
class FTP {
 private:
  // file handler
  FileHandler filehandler_;

  // flag if debugging should be enabled (true) or not
  boolean debug_ = false;

  // FTP server IP address
  IPAddress ftpserver_;
  // FTP server port
  uint16_t ftpport_;

  // FTP client
  WiFiClient ftpclient_;
  // FTP client for data port
  WiFiClient ftpdclient_;
  // output buffer
  char ftpoutBuf_[128];

 public:
  // Constructor
  // parameters:
  //   ftpserverip ... IP address of the FTP server to connect to
  //   ftpport ... port numnber of the FTP server to connect to
  //   debug ... flag if debugging should be enabled (true) or not
  FTP(IPAddress ftpserverip = FTP_SERVER_IP_ADDRESS,
      uint16_t ftpport = FTP_SERVER_PORT, boolean debug = FTP_DEBUG_ENABLED);

  // Uploads the file via FTP
  // parameters
  //   filename ... name of the file to upload
  byte ftpUploadFile(String filename);

  // handler for FTP errors
  void efail();

  // handler for received data
  byte eRcv();
};

}  // namespace arduino_temp