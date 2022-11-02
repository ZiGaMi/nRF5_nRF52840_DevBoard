# Changelog
All notable changes to this project/module will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project/module adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

### Todo
 - 

### Memory usage:
 - RAM: 21.5kB/256kB (8.4%)
 - FLASH: 50.9kB/1024kB (4.9%)

---