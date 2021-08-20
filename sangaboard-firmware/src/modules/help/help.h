#include "config.h"
#ifdef HELP
#ifndef HELP_H
#include "main.h"
#include <Arduino.h>

void help(String);
void help_setup();
const struct Command help_commands[] = {
    {"help", help},
    END_COMMAND};
#define HELP_H
#endif
#endif