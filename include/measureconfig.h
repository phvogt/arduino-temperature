#pragma once

#include <Arduino.h>

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

// Configuration for measuring.
class MeasureConfig {
 public:
  // type of DHT
  uint8_t dhtType;
  // pin the DHT is connected
  uint8_t dhtPin;
  // flag if values should be simulated (true) or not (false)
  boolean isSimulateValuesEnabled;

  // factor for calculating the voltage of the battery from the measured value
  double batteryFactor;

  // max retries to read values
  int dhtMaxRetries;
  // delay in milliseconds between retries
  unsigned long dhtRetryDelayInMillis;
};

}  // namespace arduino_temp