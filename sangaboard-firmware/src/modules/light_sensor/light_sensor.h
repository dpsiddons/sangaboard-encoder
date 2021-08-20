#include "config.h"
#ifdef LIGHT_SENSOR
#ifndef LIGHT_SENSOR_H
#include <Arduino.h>

void light_sensor_gain(String);
void light_sensor_gain_values(String);
void light_sensor_integration_time(String);
void light_sensor_intensity(String);

void light_sensor_setup();
void setup_light_sensor_device();

void print_light_sensor_gain();
void set_light_sensor_gain(int);
#define LIGHT_SENSOR_H
#endif
#endif