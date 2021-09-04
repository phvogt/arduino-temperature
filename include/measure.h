#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "logger.h"
#include "measureconfig.h"

namespace arduino_temp {

// Measure with the sensor.
class Measure {
 private:
  // measuring configuration
  MeasureConfig measureConfig_;
  // logging
  Logger logger_;

  // reference to the DHT sensor
  DHT dht_;

  // Gets the temperature / humidity and sends it to MQTT.
  // parameters:
  // returns the measured values
  measured_values_dht getAndSendTemperatureAndHumidityData();

 public:
  // Constructor.
  // parameters:
  //   measureConfig ... measuring configuration
  //   logger ... logging
  Measure(MeasureConfig measureConfig, Logger logger);

  // initialize DHT temperature / humidity sensor
  void initDHT();

  // Gets the temperature and humidity from the DHT.
  // parameters: none
  // returns the struct with the measured values from the DHT
  measured_values_dht measureDHTTempAndHumidity();

  // Gets the battery values
  // parameters: none
  // returns the battery status
  measured_values_bat measureBatteryValues();
};

}  // namespace arduino_temp