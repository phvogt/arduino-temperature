#include "measure.h"

#include "filehandler.h"

arduino_temp::Measure::Measure(MeasureConfig measureConfig)
    : measureConfig_(measureConfig),
      dht_(measureConfig.dhtPin, measureConfig.dhtType) {}

void arduino_temp::Measure::initDHT() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Init DHT");
  dht_.begin();
}

measured_values_dht arduino_temp::Measure::measureDHTTempAndHumidity() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Collecting temp / hum");

  // measure temperature / humidity
  struct measured_values_dht measuredValuesDht =
      getAndSendTemperatureAndHumidityData();
  if (!measuredValuesDht.couldReadValues) {
    FileHandler::getInstance().doLogWarn("Failed to read from DHT sensor!");
  }
  return measuredValuesDht;
}

measured_values_dht
arduino_temp::Measure::getAndSendTemperatureAndHumidityData() {
  struct measured_values_dht measuredValues;

  measuredValues.couldReadValues = false;
  measuredValues.temp = 0;
  measuredValues.hum = 0;

  // should we simulate values for testing?
  if (measureConfig_.isSimulateValuesEnabled) {
    measuredValues.couldReadValues = true;
    measuredValues.temp = 21.5;
    measuredValues.hum = 50.5;

  } else {
    // get real values from DHT
    int triedToReadTimes = 0;

    while (!measuredValues.couldReadValues &&
           (triedToReadTimes < measureConfig_.dhtMaxRetries)) {
      // Reading temperature or humidity takes about 250 milliseconds!
      measuredValues.hum = dht_.readHumidity();
      // Read temperature as Celsius (the default)
      measuredValues.temp = dht_.readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(measuredValues.hum) && isnan(measuredValues.temp)) {
        FileHandler::getInstance().doLogWarn("Failed to read from DHT sensor!");
        triedToReadTimes++;
        delay(measureConfig_.dhtRetryDelayInMillis);
      } else {
        measuredValues.couldReadValues = true;
      }
    }
  }

  String temperature = String(measuredValues.temp);
  String humidity = String(measuredValues.hum);

  FileHandler::getInstance().doLogInfo("Temperature: " + temperature +
                                       " *C, Humidity: " + humidity);
  FileHandler::getInstance().doLogInfoLine();

  return measuredValues;
}

measured_values_bat arduino_temp::Measure::measureBatteryValues() {
  struct measured_values_bat measuredValues;

  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Checking battery");

  measuredValues.batt = analogRead(A0);

  // calculate the battery
  measuredValues.battV =
      (double)measuredValues.batt / 1023.0 * measureConfig_.batteryFactor;

  FileHandler::getInstance().doLogInfo("  batt: " +
                                       String(measuredValues.batt));
  FileHandler::getInstance().doLogInfo("  batteryFactor: " +
                                       String(measureConfig_.batteryFactor));
  FileHandler::getInstance().doLogInfo("  battV: " +
                                       String(measuredValues.battV));

  FileHandler::getInstance().doLogInfoLine();

  return measuredValues;
}
