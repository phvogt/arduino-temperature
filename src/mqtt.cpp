#include "mqtt.h"

arduino_temp::MQTT::MQTT(MQTTConfig& mqttConfig, Logger& logger) {
  mqttConfig_ = &mqttConfig;
  logger_ = &logger;
  mqttClient_ = new MQTTClient();
}

boolean arduino_temp::MQTT::setupMQTT() {
  logger_->logInfoLine();
  logger_->logInfo("setup MQTT");
  logger_->logInfoLine();
  logger_->logInfo("MQTT connecting to " + String(mqttConfig_->host) + ":" +
                  mqttConfig_->port);

  mqttClient_->begin(mqttConfig_->host, mqttConfig_->port, mqttNet_);

  logger_->logInfo("connecting...");

  int mqttWifiCount = 0;
  while (!mqttClient_->connect(mqttConfig_->host, mqttConfig_->username,
                              mqttConfig_->password) &&
         (mqttWifiCount < mqttConfig_->connectionMaxRetries)) {
    logger_->logInfo(".");
    delay(mqttConfig_->connectionRetryDelayInMillis);
    mqttWifiCount++;
  }
  logger_->logInfo("");

  boolean isMqttConnected = mqttClient_->connected();

  if (isMqttConnected) {
    logger_->logInfo("MQTT connected");
  } else {
    logger_->logWarn("MQTT not connected!");
  }
  logger_->logInfoLine();

  return isMqttConnected;
}

String arduino_temp::MQTT::getTopic() { return mqttConfig_->topic; }

void arduino_temp::MQTT::sendMqtt(String topic, String message) {
  logger_->logInfo("MQTT sending: to \"" + topic + "\": \"" + message +
                  "\" (length: " + message.length() + ")");

  // only send message, if connected
  if (mqttClient_->connected()) {
    // process messages
    mqttClient_->loop();

    // convert parameter
    char c_topic[topic.length() + 1];
    topic.toCharArray(c_topic, topic.length() + 1);

    char c_message[message.length() + 1];
    message.toCharArray(c_message, message.length() + 1);

    // send message
    int returnstatus =
        mqttClient_->publish(c_topic, c_message, message.length());

    if (returnstatus != 1) {
      logger_->logInfo("  not sent! returnstatus = " + String(returnstatus));
    }
  } else {
    logger_->logWarn("  not connected, so not sending.");
  }
}
