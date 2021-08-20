#ifndef MAIN_H
#include "config.h"
#include <Arduino.h>
#ifdef DEBUG_ON
#define D(x) Serial.println(x)
#else
#define D(x)
#endif

#define END_COMMAND \
    {               \
        "", NULL    \
    }
#define CHECK_END_COMMAND(C) check_end_command(C)

struct Command
{
    const char * command;
    void (*run)(String);
};

inline bool check_end_command(Command c) { return strlen(c.command) > 0; }
inline bool check_end_command(Command * c) {return strlen(c->command) > 0; }

const Command end_command = END_COMMAND;
uint8_t parse_arguments(char ** arguments, String, uint8_t);
extern void register_module(const Command commands[], void (*loop_fn)(void));

void get_version(String);
void get_modules(String);
#define MAIN_H
#endif