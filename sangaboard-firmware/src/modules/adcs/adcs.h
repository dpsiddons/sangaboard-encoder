#include "config.h"
#ifdef ADCS
#ifndef ADCS_H
#include "config.h"

#include "main.h"
#include <Arduino.h>

void adcs_setup();
// void adcs_loop();

extern const Command adc_commands[];

void adc_read(void);
void adc_device(uint8);
void which_device(void);
const uint8_t adcs[]={0,1,2,3};
#define ADCS_H
#endif
#endif