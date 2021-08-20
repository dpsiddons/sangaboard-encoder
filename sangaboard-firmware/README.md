# Firmware for the Sangaboard and similar motor controllers

## This firmware is a Work in Progress, do NOT use for production builds

Many features of this firmware are untested and it changes behaviour of the pysangaboard module, breaking fast autofocus in OFM software (see #1, !1). If you want a reliable device use [the original firmware](https://gitlab.com/bath_open_instrumentation_group/sangaboard/-/blob/master/arduino_code). 

If you like living on the edge and decide to use this firmware, please report any issues you run into to this repo.

## About
Firmware for the control board of OpenFlexure Microscope. This is a refactor of the [original arduino code](https://gitlab.com/bath_open_instrumentation_group/sangaboard/-/blob/master/arduino_code) with some improvements to the infrastructure and features.

## Notable changes
This version of the firmware uses PlatformIO rather than Arduino IDE.

The code is split into the main part which mostly handles command parsing and modules which execute actual commands, allowing the firmware to be easily extensible with new features (add a module based on one of the existing ones and add a line to setup to register its commands and loop function).

Motor moves now do not block command processing and a new command `stop` was added which aborts any move in progress.

## Hardware
This is mainly intended for Sangaboard control boards and all configuration for these is already in `boards.h`, but it can also be used on different platforms (pi pico, stm32 (bluepill)). To use a custom board based on any of these platforms adjust `boards.h` to match your wiring.

If you have additional hardware connected to the board (Endstops, Light sensor) support for these can be enabled in `config.h`.

## Building and uploading
The easiest way to build and upload this firmware is to install `pio core` via pip. For arduino nano with sangaboard v0.2 run
```
pip3 install platformio
pio lib install
pio run -e nano
pio run -e nano -t upload
```
for sangaboard v0.3 replace `nano` with `leonardo` in the above commands (untested).

## TODO list

### Original firmware feature parity
  - [x] Implement Endstops module
  - [x] Implement Light sensor modules
  - [x] Handle Sangaboardv3

### Testing
  - [ ] Ensure responses to commands match the original version
  - [ ] Test endstop modules
  - [ ] Test Light sensor module
  - [ ] Test on different platforms

### Improvements and fixes
  - [ ] Add a CI script
  - [x] Add test for argument parsing
  - [ ] Add more unit tests
  - [x] Test automatic building and upload
  - [x] Support other platforms
  - [ ] Clean up
  - [ ] Improve handling of unused modules/libraries
  - [x] Lower RAM use
