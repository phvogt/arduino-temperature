#include "measure.h"

#include "config.h"
#include "constants.h"
#include "filehandler.h"

arduino_temp::Measure::Measure() : dht_(DHTPIN, DHTTYPE) { ; }

void arduino_temp::Measure::initDHT() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Init DHT");
  dht_.begin();
}

// Gets the temperature and humidity from the DHT.
// parameters: none
// returns the struct with the measured values from the DHT
measured_values_dht arduino_temp::Measure::measureDHTTempAndHumidity() {
  FileHandler::getInstance().doLogInfoLine();
  FileHandler::getInstance().doLogInfo("Collecting temp / hum for room: " +
                                       MQTT_ROOM);

  // measure temperature / humidity
  struct measured_values_dht measuredValuesDht =
      getAndSendTemperatureAndHumidityData(DHT_SIMULATE_VALUES,
                                           DHT_MAX_READ_VALUES_RETRIES,
                                           DHT_DELAY_IN_MILLIS);
  if (!measuredValuesDht.couldReadValues) {
    FileHandler::getInstance().doLogWarn("Failed to read from DHT sensor!");
  }
  return measuredValuesDht;
}

measured_values_dht arduino_temp::Measure::getAndSendTemperatureAndHumidityData(
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

    while (!measuredValues.couldReadValues &&
           (triedToReadTimes < dhtMaxReadValuesRetries)) {
      // Reading temperature or humidity takes about 250 milliseconds!
      measuredValues.hum = dht_.readHumidity();
      // Read temperature as Celsius (the default)
      measuredValues.temp = dht_.readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(measuredValues.hum) && isnan(measuredValues.temp)) {
        FileHandler::getInstance().doLogWarn("Failed to read from DHT sensor!");
        triedToReadTimes++;
        delay(dhtRetryDelayInMillis);
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

  // factor for calculating battery voltage
  // voltage divider: 1 volt = 1024, divider is
  const double batteryFactor =
      (BATTERY_RESISTOR_ADC_GND_VALUE + BATTERY_RESISTOR_ADC_BATT_VALUE) /
      BATTERY_RESISTOR_ADC_GND_VALUE;

  measuredValues.batt = analogRead(A0);

  // calculate the battery
  measuredValues.battV = (double)measuredValues.batt / 1023.0 * batteryFactor;

  FileHandler::getInstance().doLogInfo("  batt: " +
                                       String(measuredValues.batt));
  FileHandler::getInstance().doLogInfo("  batteryFactor: " +
                                       String(batteryFactor));
  FileHandler::getInstance().doLogInfo("  battV: " +
                                       String(measuredValues.battV));

  FileHandler::getInstance().doLogInfoLine();

  return measuredValues;
}
