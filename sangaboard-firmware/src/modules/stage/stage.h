#ifndef STAGE_H
#include "StepperF_alt.h"
#include "main.h"
#include <Arduino.h>

#define EACH_MOTOR for (int i = 0; i < STAGE_N_MOTORS; i++)

void stage_setup();
void stage_loop();

void print_position();

void stage_mrx(String command);
void stage_mry(String command);
void stage_mrz(String command);
void stage_mr(String command);
void stage_release(String command);
void stage_p(String command);
void stage_min_step_delay(String command);
void stage_ramp_time(String command);
void stage_zero(String command);
void start_move(long displ[]);
void stage_stop(String command);

extern const Command stage_commands[];
//we want to expose these for endstops (and potentially other modules)
extern signed long current_pos[];
extern long displacement[];
extern bool stage_moving;
extern int8_t move_directions[];
extern unsigned long distance_moved[];
extern long min_step_delay;

#define STAGE_H
#endif