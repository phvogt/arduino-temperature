#include "all_headers.h"

WiFiClient global_mqttNet;
MQTTClient global_mqttClient;

// initialize the MQTT client
// parameters:
//   host ... MQTT host
//   port ... MQTT port (1883 for no SSL or 8883 for SSL)
//   username ... MQTT username
//   password ... MQTT password
// returns true if the MQTT client is connected
boolean initMqtt(String host, const int port, String username, String password,
		int maxMqttConnectionRetries,
		unsigned long mqttConnectRetryDelayInMillis) {

	// convert parameters for library
	char c_host[host.length() + 1];
	host.toCharArray(c_host, host.length() + 1);
	char c_username[username.length() + 1];
	username.toCharArray(c_username, username.length() + 1);
	char c_password[password.length() + 1];
	password.toCharArray(c_password, password.length() + 1);

	doLogInfo("------------------------------------------------------");
	doLogInfo("MQTT connecting to " + host + ":" + port);

	global_mqttClient.begin(c_host, port, global_mqttNet);

	doLogInfo("connecting...");

	int mqttWifiCount = 0;
	while (!global_mqttClient.connect(c_host, c_username, c_password)
			&& (mqttWifiCount < maxMqttConnectionRetries)) {
		doLogInfo(".");
		delay(mqttConnectRetryDelayInMillis);
		mqttWifiCount++;
	}
	doLogInfo("");

	boolean isMqttConnected = global_mqttClient.connected();

	if (isMqttConnected) {
		doLogInfo("MQTT connected");
	} else {
		doLogWarn("MQTT not connected!");
	}
	doLogInfo("------------------------------------------------------");

	return isMqttConnected;
}

// Send MQTT message.
// parameters:
// topic ... topic
// message ... message
void sendMqtt(String topic, String message) {

	doLogInfo(
			"MQTT sending: to \"" + topic + "\": \"" + message + "\" (length: "
					+ message.length() + ")");

	// only send message, if connected
	if (global_mqttClient.connected()) {

		// process messages
		global_mqttClient.loop();

		// convert parameter
		char c_topic[topic.length() + 1];
		topic.toCharArray(c_topic, topic.length() + 1);

		char c_message[message.length() + 1];
		message.toCharArray(c_message, message.length() + 1);

		// send message
		int returnstatus = global_mqttClient.publish(c_topic, c_message,
				message.length());

		if (returnstatus != 1) {
			doLogInfo("  not sent! returnstatus = " + String(returnstatus));
		}
	} else {
		doLogWarn("  not connected, so not sending.");
	}

}

