#include "mqtt.h"

#include "filehandler.h"

arduino_temp::MQTT::MQTT(MQTTConfig mqttConfig) : mqttConfig_(mqttConfig) {}

boolean arduino_temp::MQTT::setupMQTT() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("setup MQTT");
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("MQTT connecting to " +
                                       String(mqttConfig_.host) + ":" +
                                       mqttConfig_.port);

  mqttClient_.begin(mqttConfig_.host, mqttConfig_.port, mqttNet_);

  FileHandler::getInstance().doLogInfo("connecting...");

  int mqttWifiCount = 0;
  while (!mqttClient_.connect(mqttConfig_.host, mqttConfig_.username,
                              mqttConfig_.password) &&
         (mqttWifiCount < mqttConfig_.connectionMaxRetries)) {
    FileHandler::getInstance().doLogInfo(".");
    delay(mqttConfig_.connectionRetryDelayInMillis);
    mqttWifiCount++;
  }
  FileHandler::getInstance().doLogInfo("");

  boolean isMqttConnected = mqttClient_.connected();

  if (isMqttConnected) {
    FileHandler::getInstance().doLogInfo("MQTT connected");
  } else {
    FileHandler::getInstance().doLogWarn("MQTT not connected!");
  }
  FileHandler::getInstance().doLogInfoLine();

  return isMqttConnected;
}

String arduino_temp::MQTT::getTopic() { return mqttConfig_.topic; }

void arduino_temp::MQTT::sendMqtt(String topic, String message) {
  FileHandler::getInstance().doLogInfo("MQTT sending: to \"" + topic +
                                       "\": \"" + message +
                                       "\" (length: " + message.length() + ")");

  // only send message, if connected
  if (mqttClient_.connected()) {
    // process messages
    mqttClient_.loop();

    // convert parameter
    char c_topic[topic.length() + 1];
    topic.toCharArray(c_topic, topic.length() + 1);

    char c_message[message.length() + 1];
    message.toCharArray(c_message, message.length() + 1);

    // send message
    int returnstatus =
        mqttClient_.publish(c_topic, c_message, message.length());

    if (returnstatus != 1) {
      FileHandler::getInstance().doLogInfo("  not sent! returnstatus = " +
                                           String(returnstatus));
    }
  } else {
    FileHandler::getInstance().doLogWarn("  not connected, so not sending.");
  }
}
