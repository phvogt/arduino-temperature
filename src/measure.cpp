#include "measure.h"

arduino_temp::Measure::Measure(MeasureConfig& measureConfig, Logger& logger) {
  measureConfig_ = &measureConfig;
  logger_ = &logger;
  dht_ = new DHT(measureConfig.dhtPin, measureConfig.dhtType);
}

void arduino_temp::Measure::initDHT() {
  logger_->logInfoLine();
  logger_->logInfo("Init DHT");
  dht_->begin();
}

measured_values_dht arduino_temp::Measure::measureDHTTempAndHumidity() {
  logger_->logInfoLine();
  logger_->logInfo("Collecting temp / hum");

  // measure temperature / humidity
  struct measured_values_dht measuredValuesDht =
      getAndSendTemperatureAndHumidityData();
  if (!measuredValuesDht.couldReadValues) {
    logger_->logWarn("Failed to read from DHT sensor!");
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
  if (measureConfig_->isSimulateValuesEnabled) {
    measuredValues.couldReadValues = true;
    measuredValues.temp = 21.5;
    measuredValues.hum = 50.5;

  } else {
    // get real values from DHT
    int triedToReadTimes = 0;

    while (!measuredValues.couldReadValues &&
           (triedToReadTimes < measureConfig_->dhtMaxRetries)) {
      // Reading temperature or humidity takes about 250 milliseconds!
      measuredValues.hum = dht_->readHumidity();
      // Read temperature as Celsius (the default)
      measuredValues.temp = dht_->readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(measuredValues.hum) && isnan(measuredValues.temp)) {
        logger_->logWarn("Failed to read from DHT sensor!");
        triedToReadTimes++;
        delay(measureConfig_->dhtRetryDelayInMillis);
      } else {
        measuredValues.couldReadValues = true;
      }
    }
  }

  String temperature = String(measuredValues.temp);
  String humidity = String(measuredValues.hum);

  logger_->logInfo("Temperature: " + temperature + " *C, Humidity: " + humidity);
  logger_->logInfoLine();

  return measuredValues;
}

measured_values_bat arduino_temp::Measure::measureBatteryValues() {
  struct measured_values_bat measuredValues;

  logger_->logInfoLine();
  logger_->logInfo("Checking battery");

  measuredValues.batt = analogRead(A0);

  // calculate the battery
  measuredValues.battV =
      (double)measuredValues.batt / 1023.0 * measureConfig_->batteryFactor;

  logger_->logInfo("  batt: " + String(measuredValues.batt));
  logger_->logInfo("  batteryFactor: " + String(measureConfig_->batteryFactor));
  logger_->logInfo("  battV: " + String(measuredValues.battV));

  logger_->logInfoLine();

  return measuredValues;
}
