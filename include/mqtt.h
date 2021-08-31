#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTTClient.h>

#include "filehandler.h"

namespace arduino_temp {

// MQTT client to transfer information.
class MQTT {
 private:
  // file handler
  FileHandler filehandler_;

  // MQTT host
  char* host_;
  // MQTT port (1883 for no SSL or 8883 for SSL)
  int port_;
  // MQTT username
  char* username_;
  // MQTT password
  char* password_;

  // maximum number of retires to connect to MQTT
  int maxMqttConnectionRetries_;
  // delay in milliseconds between connection retries
  unsigned long mqttConnectRetryDelayInMillis_;

  // WIFI client
  WiFiClient mqttNet_;
  // MQTT client
  MQTTClient mqttClient_;

 public:
  // parameters:
  //   host ... MQTT host
  //   port ... MQTT port (1883 for no SSL or 8883 for SSL)
  //   username ... MQTT username
  //   password ... MQTT password
  // returns true if the MQTT client is connected
  MQTT(String host, const int port, String username, String password,
       int maxMqttConnectionRetries,
       unsigned long mqttConnectRetryDelayInMillis);

  // free members
  ~MQTT();

  // Setups of the MQTT.
  // returns the MQTT connection status
  boolean setupMQTT();

  // Send MQTT message.
  // parameters:
  // topic ... topic
  // message ... message
  void sendMqtt(String topic, String message);
};

}  // namespace arduino_temp