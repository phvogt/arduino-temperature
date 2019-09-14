#include "all_headers.h"

// for measuring timing
unsigned long global_timingStart = 0;

// contains all the timings
String global_timings = "";


// Getting the reset reason.
// parameters: none
// returns the reset reason
String getResetReason() {

	doLogInfo("======================================================");
	doLogInfo("getting reset reason");

	String reason = ESP.getResetReason();

	doLogInfo("reason: " + reason);

	doLogInfo("======================================================");

	return reason;
}

// Calculate the sleep time.
// parameters:
//   sleepTimeInMillis ... time to sleep in milli seconds
//   startTimeInMillis ... start time in milli seconds
// return the time to sleep in micro seconds
long calcSleepTimeInMicroSeconds(int sleepTimeInMillis, int startTimeInMillis) {

	doLogInfo("======================================================");
	doLogInfo("calculating sleep time");

	// calculate the already worked millis
	long currentMillis = millis();
	long workedTimeMillis = currentMillis - startTimeInMillis;
	if (workedTimeMillis < 0) {
		workedTimeMillis = 0;
	}

	long sleepTimeMicro = sleepTimeInMillis * 1000 - workedTimeMillis * 1000;
	doLogInfo(
			"startTimeInMillis: " + String(startTimeInMillis)
					+ " sleepTimeInMillis: " + String(sleepTimeInMillis)
					+ " sleepTimeMicro: " + String(sleepTimeMicro));
	doLogInfo("======================================================");

	return sleepTimeMicro;
}

// Do the sleeping for the provided micro seconds.
// parameters:
//   sleepTimeInMicros ... the sleep time in micro seconds
// returns nothing
void doSleepForMicros(long sleepTimeInMicros) {

	ESP.deepSleep(sleepTimeInMicros, WAKE_RF_DISABLED);
	delay(100);
}

// Sleeps fot the maximum time.
// parameters: none
// returns nothing
void sleepMaxTime() {

	int sleepTimeMicro = CORE_SLEEP_TIME_IN_MILLIS * 1000;
	doLogInfo("going max time sleeping for " + String(sleepTimeMicro));
	doSleepForMicros(sleepTimeMicro);

}

// Starts the timing measuring.
// parameters: none
// returns nothing
void timingStart() {
	global_timingStart = millis();
}

// Stops the timing measuring and logs the duration.
// parameters:
//   measuredName ... name of the duration that was measured
// returns the difference between timingStart() and timingEnd() in milliseconds.
unsigned long timingEnd(String measuredName) {

	unsigned long duration = millis() - global_timingStart;
	String logstring = measuredName + ": " + String(duration);

	doLogTime(logstring);

	if (global_timings == "") {
		global_timings += logstring;
	} else {
		global_timings += "," + logstring;
	}

	return duration;
}

// Gets the timings so far.
// parameters: none
// returns the timings
String getTimings() {
	return global_timings;
}

