#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace arduino_temp {

// FTP configuration.
class FTPConfig {
 public:
  // flag if debugging should be enabled (true) or not
  boolean isDebugEnabled = false;

  // FTP server IP address
  IPAddress server;
  // FTP server port
  uint16_t port;

  // username for FTP connection
  String username;
  // password for FTP connection
  String password;
};

}  // namespace arduino_temp