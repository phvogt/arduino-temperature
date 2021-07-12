#include "all_headers.h"

boolean global_log_enabled = false;
unsigned long global_startMillis = 0;

void setup() {

	// store the start time
	global_startMillis = millis();

	// setup serial connection
	Serial.begin(BAUDRATE);
	delay(10);

	Serial.println();
	Serial.println();

	// initially turn off wifi
	doLogInfo("------------------------------------------------------");
	doLogInfo("Turning Wifi off");
	initWifiOff();

	doLogInfo("------------------------------------------------------");
	doLogInfo("Starting with work mode: " + String(WORK_MODE));

	if (WORK_MODE != WORK_MODE_NORMAL) {
		startFS();
	}

	boolean connected = false;

	switch (WORK_MODE) {
	case WORK_MODE_NORMAL:
		doWorkNormal();
		break;
	case WORK_MODE_LIST_DIRECTORY:
		listDirectory (LOGFILE_DIRECTORY);
		break;
	case WORK_MODE_PRINT_LOGFILES:
		printLogfiles(LOGFILE_NAME, LOGFILE_MAX_BACKUPS);
		break;
	case WORK_MODE_DELETE_LOGFILES:
		deleteLogfiles(LOGFILE_DIRECTORY, LOGFILE_NAME);
		break;
	case WORK_MODE_PRINT_FILE:
		printFileToSerial (PRINT_FILE_NAME);
		break;
	case WORK_MODE_FTPUPLOAD:
		connected = connectWifi();
		if (connected) {
			ftpUploadFile (LOGFILE_NAME);
		}
		break;
	default:
		doLogError("unknown work mode: " + String(WORK_MODE));
		doLogInfo("going to sleep");
		break;
	}

	if (WORK_MODE != WORK_MODE_NORMAL) {
		stopFS();
		sleepMaxTime();
	}

}

// Initializes the DHT sensor.
// parameters: none
// returns nothing
void initDHT() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Init DHT");

	// initialize DHT temperature / humidity sensor
	global_dht.begin();
}

// Connect to Wifi.
// parameters: none
// returns connection status
boolean connectWifi() {
	doLogInfo("------------------------------------------------------");
	doLogInfo("Connecting WIFI");
	timingStart();

	int wifiConnectionStatus = initWifi(WIFI_SSID, WIFI_SSID_PASSWORD,
			WIFI_MAX_RETRIES, WIFI_CONNECT_RETRY_DELAY_IN_MILLIS,
			WIFI_RETRY_RESET_COUNT);
	boolean connected = (wifiConnectionStatus == WL_CONNECTED);

	timingEnd("WLAN");

	return connected;
}

// Initialize the time from NTP.
// parameters: none
// returns nothing
void initNTP() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("getting NTP");

	timingStart();
	doLogInfo("  calling begin");
	global_timeClient.begin();
	doLogInfo("  getting NTP time");
	time_t currentEpoch = getNtpTime(NTP_MAX_RETRIES,
			NTP_RETRY_DELAY_IN_MILLIS);
	doLogInfo("  setting time. currentEpoch = " + String(currentEpoch));
	setTime(currentEpoch);
	timingEnd("NTP");
}

// Setups of the MQTT.
// parameters: none
// returns the MQTT connection status
boolean setupMQTT() {
	doLogInfo("------------------------------------------------------");
	doLogInfo("setup MQTT");
	timingStart();
	boolean mqttConnected = initMqtt(MQTT_HOST, MQTT_PORT, MQTT_USERNAME,
			MQTT_PASSWORD, MQTT_MAX_RETRIES,
			MQTT_CONNECT_RETRY_DELAY_IN_MILLIS);
	timingEnd("MQI");

	return mqttConnected;
}

// Get the reset reason.
// parameters: none
// returns the reset reason
String determineResetReason() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Getting reset reason");

	timingStart();
	// get the reason the esp was reset
	String resetReason = getResetReason();
	doLogInfo("resetReason = " + resetReason);
	timingEnd("RST");

	return resetReason;
}

// Get the battery values.
// parameters: none
// returns the structure with the measured values
struct measured_values_bat measureBatteryValues() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Measuring battery values: ");

	timingStart();

	// factor for calculating battery voltage
	// voltage divider: 1 volt = 1024, divider is
	const double BATTERY_FACTOR = (BATTERY_RESISTOR_ADC_GND_VALUE
			+ BATTERY_RESISTOR_ADC_BATT_VALUE) / BATTERY_RESISTOR_ADC_GND_VALUE;

	// get battery values
	struct measured_values_bat measuredValuesBatt = getBattery(BATTERY_FACTOR);
	timingEnd("BT");

	return measuredValuesBatt;
}

// Gets the temperature and humidity from the DHT.
// parameters: none
// returns the struct with the measured values from the DHT
struct measured_values_dht measureDHTTempAndHumidity() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Collecting temp / hum for room: " + MQTT_ROOM);

	timingStart();
	// measure temperature / humidity
	struct measured_values_dht measuredValuesDht =
			getAndSendTemperatureAndHumidityData(DHT_SIMULATE_VALUES,
					DHT_MAX_READ_VALUES_RETRIES, DHT_DELAY_IN_MILLIS);
	if (!measuredValuesDht.couldReadValues) {
		doLogWarn("Failed to read from DHT sensor!");
	}
	timingEnd("DHT");

	return measuredValuesDht;
}

// Renames the log file with the last bad extension.
// parameters: none
// returns nothing
void renameLogLastBad() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("rename logfile with last bad extension");

	timingStart();
	doLogInfo("saving log file to " + LOGFILE_NAME + LOGFILE_LASTBAD_EXTENSION);
	renameFile(LOGFILE_NAME, LOGFILE_NAME + LOGFILE_LASTBAD_EXTENSION);
	timingEnd("RLOG");

}

// Sends the measured values via MQTT.
// parameters:
//   resetReason ... the reset reason
//   measured_values_bat ... the measured value for the battery
//   measured_values_dht ... the measured value with MQTT
// returns nothing
void sendMQTTValues(const String& resetReason,
		const struct measured_values_bat& measuredValuesBatt,
		const struct measured_values_dht& measuredValuesDht) {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Sending data via MQTT for room: " + MQTT_ROOM);

	timingStart();
	// send all values
	// wifi connection time
	sendMqtt(MQTT_ROOM + "/wificonnection", String(global_wifiMillis));
	// date time
	sendMqtt(MQTT_ROOM + "/datetime", getDateTime());
	// reset reason
	sendMqtt(MQTT_ROOM + "/resetReason", resetReason);
	// battery
	sendMqtt(MQTT_ROOM + "/batt", String(measuredValuesBatt.battV));
	sendMqtt(MQTT_ROOM + "/battRaw", String(measuredValuesBatt.batt));
	// temperature / humidity values
	if (measuredValuesDht.couldReadValues) {
		sendMqtt(MQTT_ROOM + "/temperature", String(measuredValuesDht.temp));
		sendMqtt(MQTT_ROOM + "/humidity", String(measuredValuesDht.hum));
	}
	timingEnd("MQV");
}

// Send the whole duration via MQTT.
// parameters:
//   startMillis ... the milliseconds since the start of the program
// returns the duration
long sendMQTTDuration(unsigned long startMillis) {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Calculatin duration");

	timingStart();
	long wholeduration = millis() - startMillis;
	doLogInfo("duration: " + String(wholeduration) + " ms");
	sendMqtt(MQTT_ROOM + "/duration", String(wholeduration));
	timingEnd("MQD");
	doLogTime("all: " + String(wholeduration));

	return wholeduration;
}

// Calculates the time to sleep.
// parameters:
//   startMillis ... the milliseconds since the start of the program
// returns the calculated sleep time in micro seconds
long calculateSleepTimeMicros(unsigned long startMillis) {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Calculating sleep");

	timingStart();
	// calculate sleeping time
	long sleepTimeInMicros = calcSleepTimeInMicroSeconds(
			CORE_SLEEP_TIME_IN_MILLIS, startMillis);
	return sleepTimeInMicros;
}

// Sends the sleep time in milliseconds.
// parameters:
//   sleepTimeInMicros ... the sleep time in microseconds
// returns nothing
void sendMQTTSleepTime(long sleepTimeInMicros) {
	doLogInfo("sleeping for microseconds: " + String(sleepTimeInMicros));
	sendMqtt(MQTT_ROOM + "/sleepTime", String(sleepTimeInMicros / 1000));
	timingEnd("MQS");
}

// Send the timings via MQTT.
// parameters: none
// returns nothing
void sendMQTTTimings() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Sending timings");

	String timingsstr = getTimings() + ",A:"
			+ String(millis() - global_startMillis);
	timingsstr.replace(" ", "");
	sendMqtt(MQTT_ROOM + "/timings", timingsstr);
}

// Start the file system and enable logging.
// parameters: none
// returns nothing
void startFS() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Starting LittleFS");

	timingStart();
	// setup LittleFS to write files to flash file system
	LittleFS.begin();
	timingEnd("FS");
}

// Disables logging to file system and stops the file system.
// parameters: none
// returns nothing
void stopFS() {

	doLogInfo("------------------------------------------------------");
	doLogInfo("Stopping LittleFS");

	global_log_enabled = false;
	LittleFS.end();
}

// Dump the current log file buffer to the log file.
// parameters: none
// returns nothing
void dumpLog() {

	doLogInfo("------------------------------------------------------");

	doLogInfo("Dumping logfile");
	timingStart();
	// dump the current log buffer to the log file
	dumpLogBuffer();
	timingEnd("DUL");
}

// Rotate the log files.
// parameters: none
// returns: nothing
void rotateLogs() {

	doLogInfo("------------------------------------------------------");
	doLogInfo(" Starting rotating files");

	timingStart();
	//	rotate log files
	rotateLogfiles(LOGFILE_NAME, LOGFILE_MAX_BACKUPS);
	doLogInfo("Done rotating files.");
	timingEnd("RLOG");
}

// Renames the log file with the last good extension.
// parameters: none
// returns nothing
void renameLogGood() {

	doLogInfo("------------------------------------------------------");
	doLogInfo(
			"saving log file to " + LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION);

	timingStart();
	renameFile(LOGFILE_NAME, LOGFILE_NAME + LOGFILE_LASTGOOD_EXTENSION);
	timingEnd("LOG");
}

// Do the normal work.
void doWorkNormal() {

	doLogInfo("======================================================");
	doLogInfo("Starting work at: " + String(millis()));

	initDHT();

	const String resetReason = determineResetReason();
	struct measured_values_bat measuredValuesBatt = measureBatteryValues();
	struct measured_values_dht measuredValuesDht = measureDHTTempAndHumidity();

	boolean connected = connectWifi();
	if (connected) {
		initNTP();
		connected = setupMQTT();
	} else {
		stopWifiForSleep();
		startFS();
		deleteFile(LOGFILE_NAME);
		dumpLog();
		global_log_enabled = true;
		renameLogLastBad();
		sleepMaxTime();
	}

	sendMQTTValues(resetReason, measuredValuesBatt, measuredValuesDht);
	sendMQTTDuration (global_startMillis);

	long sleepTimeInMicros = calculateSleepTimeMicros(global_startMillis);
	sendMQTTSleepTime(sleepTimeInMicros);
	sendMQTTTimings();
	delay(50);

	// don't log if everything went fine
	// startFS();
	// rotateLogs();
	// dumpLog();
	// global_log_enabled = true;
	// renameLogGood();
	// stopFS();

	stopWifiForSleep();

	doLogInfo("------------------------------------------------------");
	doLogInfo("done, going to sleep");

	doLogInfo("======================================================");

	doSleepForMicros(sleepTimeInMicros);

}

