#include "config.h"
#ifdef ENDSTOPS
#ifndef ENDSTOPS_H
#include "config.h"
#ifndef STAGE
#error "Endstops module depends on the stage module"
#endif


#include "main.h"
#include <Arduino.h>
void endstops_setup();
void endstops_loop();

void endstops_home_min(String command);
void endstops_home_max(String command);
void endstops_max(String command);
void endstops_status(String command);
extern void stage_stop(String command);

void home_min(uint8_t axes);
void home_max(uint8_t axes);

void home_start(uint8_t, int8_t);
void home_retreat();
void home_final();
int8_t endstops_check();

extern const Command endstops_commands[];
#define ENDSTOPS_H
#endif
#endif