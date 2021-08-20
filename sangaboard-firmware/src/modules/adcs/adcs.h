#include "config.h"
#ifdef ADCS
#ifndef ADCS_H
#include "config.h"

#include "main.h"
#include <Arduino.h>

void adcs_setup();
void adcs_loop();

extern const Command adc_commands[];

void adc_read(int);
void adc_device(String);
// void which_device(void);
const uint8_t adcs[]={A0,A1,A2};
#define ADCS_H
#endif
#endif