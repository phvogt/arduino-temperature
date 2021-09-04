# Arduino ESP12E temperature sensor

This project provides the Arduino software for an ESP12E temperature sensor.

The ESP12E measures the temperature and humidity with a DHT 22 sensor and sends the values via Wifi to an MQTT server.

## Hardware

The hardware design with a step by step instruction can be found on <https://www.vogt.or.at/hardware/temperaturesensor>.

[Fritzing schematics](plans/Temperatursensor.fzz)

Top side:

![Top side small](pictures/ESP_with_ESP_voltage_regulator_topside_thumbnail.jpg)

[Top side large](pictures/ESP_with_ESP_voltage_regulator_topside.jpg)

Bottom side:

![Bottom side small](pictures/ESP_with_ESP_voltage_regulator_underside_thumbnail.jpg)

[Bottom side large](pictures/ESP_with_ESP_voltage_regulator_underside.jpg)

## 3rd party libraries

The necessary 3rd party libraries are listed in [src/arduino-temperature.cpp](src/arduino-temperature.cpp).

## Configuration

The software must be configured by creating `config.cpp` according to the instruction in [include/config.h](include/config.h). 

## Working mode

The working mode can be set in `config.cpp` with the `WORK_MODE_*` constants (see [include/constants.h](include/constants.h)).

## Logging

If no connection to the Wifi or MQTT can be made, a log file is written. The log file can be printed in the file [include/constants.h](include/constants.h) with setting the variable `WORK_MODE = WORK_MODE_PRINT_LOGFILES`.
The changed program has to be uploaded to print the log file.

## Libraries

```
name=ESP8266WiFi
version=1.0
author=Ivan Grokhotkov
category=Communication
url=

name=MQTT
version=2.5.0
author=Joel Gaehwiler <joel.gaehwiler@gmail.com>
category=Communication
url=https://github.com/256dpi/arduino-mqtt

name=DHT sensor library
version=1.4.2
author=Adafruit
category=Sensors
url=https://github.com/adafruit/DHT-sensor-library

needed for DHT
name=Adafruit Unified Sensor
version=1.1.4
author=Adafruit <info@adafruit.com>
category=Sensors
url=https://github.com/adafruit/Adafruit_Sensor


name=NTPClient
version=3.2.0
author=Fabrice Weinberg
category=Timing
url=https://github.com/arduino-libraries/NTPClient

name=Time
version=1.6.1
author=Michael Margolis
category=Timing
url=http://playground.arduino.cc/code/time
```