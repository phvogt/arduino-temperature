// Gets the date/time in the format YY-MM-DD hh:mm:ss.
// parameters: none
// returns the date time as String
String getDateTime() {

	return String(year()) + "-" + String(toStringLeadingZero(month())) + "-"
			+ String(toStringLeadingZero(day())) + " "
			+ String(toStringLeadingZero(hour())) + ":"
			+ String(toStringLeadingZero(minute())) + ":"
			+ String(toStringLeadingZero(second()));
}

// Convert the int to a double digit string with leading zeroes.
// parameters:
//   value ... the value to convert
// returns a string with the double digit value with a leading zero
String toStringLeadingZero(int value) {

	if (value < 10) {
		return "0" + String(value);
	} else {
		return String(value);
	}
}

// Get the NTP time epoch.
// parameters:
//   maxNtpRetries ... number of retries
//   ntpRetryDelayInMillis ... delay in milliseconds between retries
// returns the time as epoch
time_t getNtpTime(int maxNtpRetries, unsigned long ntpRetryDelayInMillis) {

	global_timeClient.update();

	int retryNtpCount = 1;

	unsigned long epoch = 0;

	epoch = global_timeClient.getEpochTime();

	// values below 1000 are not realistic
	while ((epoch < 1000) && (retryNtpCount <= maxNtpRetries)) {
		retryNtpCount++;
		delay(ntpRetryDelayInMillis);
		global_timeClient.update();
		epoch = global_timeClient.getEpochTime();
	}

	return epoch;
}
