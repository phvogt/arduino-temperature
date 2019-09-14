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

The necessary 3rd party libraries are listed in [arduino-temperature.ino](arduino-temperature.ino).

## Configuration

The software can be configured in [b_config.ino](b_config.ino). 

## Working mode

The working mode can be set in the file [a_constants.ino](a_constants.ino).

## Logging

If no connection to the Wifi or MQTT can be made, a log file is written. The log file can be printed with the WORK_MODE_PRINT_LOGFILES constant.
The changed program has to be uploaded to print the log file.
