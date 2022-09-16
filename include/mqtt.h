#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTTClient.h>

#include "logger.h"
#include "mqttconfig.h"

namespace arduino_temp {

// MQTT client to transfer information.
class MQTT {
 private:
  // MQTT configuration
  MQTTConfig* mqttConfig_;
  // logging
  Logger* logger_;

  // WIFI client
  WiFiClient mqttNet_;
  // MQTT client
  MQTTClient* mqttClient_;

 public:
  // Constructor
  // parameters:
  //   mqttConfig ... MQTT configuration
  //   logger ... logging
  MQTT(MQTTConfig& mqttConfig, Logger& logger);

  // Setups of the MQTT.
  // parameters: none
  // returns the MQTT connection status
  boolean setupMQTT();

  // Gets the topic for MQTT
  // returns the topic
  String getTopic();

  // Send MQTT message.
  // parameters:
  // topic ... topic
  // message ... message
  // returns nothing
  void sendMqtt(String topic, String message);
};

}  // namespace arduino_temp