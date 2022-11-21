# Changelog
All notable changes to this project/module will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project/module adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---
## V1.1.0 - xx.11.2022

### Added
 - Implementation of USB CDC class driver
 - Led behaviour:
  - LED1: Heartbeat 1Hz
  - LED2: No used...
  - LED3: USB CDC Status, OFF USB un-plugged, 1x blink USB plugged, ON USB VCP open
  - LED4: Blinks on any button press


### Memory usage:
 - RAM: 20.8kB/256kB (8.1%)
 - FLASH: 84.8kB/1024kB (8.2%)

---
## V1.0.0 - 02.11.2022

### Added
 - Implementation of Cortex Systick driver
 - Implementation of GPIO low level drivers
 - Added button submodule (reading all four user buttons)
 - Added led submodule (driving all user leds)
 - Implementation of UART low level drivers
 - Added CLI submodule on debug port via J-link
 - Added parameters submodule
 - Implmenetation of ADC low level driver
 - Prepared application ADC code
    + Reading AIN1, AIN2, AIN4, AIN5, AIN5 and AIN7 analog inputs
 - Prepared application code handler
    + 10ms, 100ms and 1000ms loop handlers
    + Button event callbacks

### Memory usage:
 - RAM: 21.5kB/256kB (8.4%)
 - FLASH: 50.9kB/1024kB (4.9%)

---