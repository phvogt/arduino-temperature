#include "mqtt.h"

#include "constants.h"

// parameters:
//   host ... MQTT host
//   port ... MQTT port (1883 for no SSL or 8883 for SSL)
//   username ... MQTT username
//   password ... MQTT password
//   maxMqttConnectionRetries ... maximum retries for MQTT connections
//   mqttConnectRetryDelayInMillis ... delay in milliseconds between retries
arduino_temp::MQTT::MQTT(String host, const int port, String username,
                         String password, int maxMqttConnectionRetries,
                         unsigned long mqttConnectRetryDelayInMillis)
    : filehandler_(LOG_ENABLED) {
  host_ = (char *)malloc(host.length() + 1);
  host.toCharArray(host_, host.length() + 1);

  port_ = port;

  username_ = (char *)malloc(username.length() + 1);
  username.toCharArray(username_, username.length() + 1);

  password_ = (char *)malloc(password.length() + 1);
  password.toCharArray(password_, password.length() + 1);

  maxMqttConnectionRetries_ = maxMqttConnectionRetries;
  mqttConnectRetryDelayInMillis_ = mqttConnectRetryDelayInMillis;
}

arduino_temp::MQTT::~MQTT() {
  free(host_);
  free(username_);
  free(password_);
}

// Setups of the MQTT.
// parameters: none
// returns the MQTT connection status
boolean arduino_temp::MQTT::setupMQTT() {
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("setup MQTT");
  filehandler_.doLogInfoLine();
  filehandler_.doLogInfo("MQTT connecting to " + String(host_) + ":" + port_);

  mqttClient_.begin(host_, port_, mqttNet_);

  filehandler_.doLogInfo("connecting...");

  int mqttWifiCount = 0;
  while (!mqttClient_.connect(host_, username_, password_) &&
         (mqttWifiCount < maxMqttConnectionRetries_)) {
    filehandler_.doLogInfo(".");
    delay(mqttConnectRetryDelayInMillis_);
    mqttWifiCount++;
  }
  filehandler_.doLogInfo("");

  boolean isMqttConnected = mqttClient_.connected();

  if (isMqttConnected) {
    filehandler_.doLogInfo("MQTT connected");
  } else {
    filehandler_.doLogWarn("MQTT not connected!");
  }
  filehandler_.doLogInfoLine();

  return isMqttConnected;
}

// Send MQTT message.
// parameters:
// topic ... topic
// message ... message
void arduino_temp::MQTT::sendMqtt(String topic, String message) {
  filehandler_.doLogInfo("MQTT sending: to \"" + topic + "\": \"" + message +
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
      filehandler_.doLogInfo("  not sent! returnstatus = " +
                             String(returnstatus));
    }
  } else {
    filehandler_.doLogWarn("  not connected, so not sending.");
  }
}
