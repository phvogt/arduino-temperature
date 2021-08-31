#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "filehandler.h"

// measured values from DHT
struct measured_values_dht {
  boolean couldReadValues;
  float temp;
  float hum;
};

// measured values from battery
struct measured_values_bat {
  int batt;
  double battV;
};

namespace arduino_temp {

// Measure with the sensor.
class Measure {
 private:
  // file handler
  FileHandler filehandler_;

  // reference to the DHT sensor
  DHT dht_;

  // Gets the temperature / humidity and sends it to MQTT.
  // parameters:
  //   simulateValues ... flag if values should be simulated (true) or not
  //   (false) dhtMaxReadValuesRetries ... max retries to read values
  //   dhtRetryDelayInMillis ... delay in milliseconds between retries
  // returns the measured values
  measured_values_dht getAndSendTemperatureAndHumidityData(
      boolean simulateValues, int dhtMaxReadValuesRetries,
      unsigned long dhtRetryDelayInMillis);

 public:
  Measure();

  // initialize DHT temperature / humidity sensor
  void initDHT();

  // Gets the temperature and humidity from the DHT.
  // returns the struct with the measured values from the DHT
  measured_values_dht measureDHTTempAndHumidity();

  // Gets the battery values
  // returns the battery status
  measured_values_bat measureBatteryValues();
};

}  // namespace arduino_temp