// Gets the temperature / humidity and sends it to MQTT.
// parameters:
//   simulateValues ... flag if values should be simulated (true) or not (false)
//   dhtMaxReadValuesRetries ... max retries to read values
//   dhtRetryDelayInMillis ... delay in milliseconds between retries
// returns the measured values
struct measured_values_dht getAndSendTemperatureAndHumidityData(
		boolean simulateValues, int dhtMaxReadValuesRetries,
		unsigned long dhtRetryDelayInMillis) {

	struct measured_values_dht measuredValues;

	measuredValues.couldReadValues = false;
	measuredValues.temp = 0;
	measuredValues.hum = 0;

	// should we simulate values for testing?
	if (simulateValues) {
		measuredValues.couldReadValues = true;
		measuredValues.temp = 21.5;
		measuredValues.hum = 50.5;

	} else {

		// get real values from DHT
		int triedToReadTimes = 0;

		while (!measuredValues.couldReadValues
				&& (triedToReadTimes < dhtMaxReadValuesRetries)) {

			// Reading temperature or humidity takes about 250 milliseconds!
			measuredValues.hum = global_dht.readHumidity();
			// Read temperature as Celsius (the default)
			measuredValues.temp = global_dht.readTemperature();

			// Check if any reads failed and exit early (to try again).
			if (isnan(measuredValues.hum) && isnan(measuredValues.temp)) {
				doLogWarn("Failed to read from DHT sensor!");
				triedToReadTimes++;
				delay(dhtRetryDelayInMillis);
			} else {
				measuredValues.couldReadValues = true;
			}
		}
	}

	String temperature = String(measuredValues.temp);
	String humidity = String(measuredValues.hum);

	doLogInfo("Temperature: " + temperature + " *C, Humidity: " + humidity);
	doLogInfo("------------------------------------------------------");

	return measuredValues;
}

// Gets the battery values
// parameters:
//   batteryFactor ... factor for battery
// returns the battery status
struct measured_values_bat getBattery(double batteryFactor) {

	struct measured_values_bat measuredValues;

	doLogInfo("------------------------------------------------------");
	doLogInfo("Checking battery");

	measuredValues.batt = analogRead(A0);

	// calculate the battery
	measuredValues.battV = (double) measuredValues.batt / 1023.0
			* batteryFactor;

	doLogInfo("  batt: " + String(measuredValues.batt));
	doLogInfo("  batteryFactor: " + String(batteryFactor));
	doLogInfo("  battV: " + String(measuredValues.battV));

	doLogInfo("------------------------------------------------------");

	return measuredValues;
}

