#pragma once

#include <Arduino.h>

namespace arduino_temp {

// MQTT configuration.
class MQTTConfig {
 public:
  // MQTT host
  const char* host;
  // MQTT port (1883 for no SSL or 8883 for SSL)
  int port;
  // MQTT username
  const char* username;
  // MQTT password
  const char* password;
  // the topic to use in the MQTT message
  String topic;

  // only try this number times to connect to MQTT (with delay below)
  int connectionMaxRetries;
  // delay between MQTT connection retries in milli seconds
  unsigned long connectionRetryDelayInMillis;
};

}  // namespace arduino_temp