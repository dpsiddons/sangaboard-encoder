/*
 * Sangaboard firmware
 *
 * Refactored from bath_open_instrumentation_group/sangaboard/arduino_code
 * 
 * This firmware was written by
 * Richard Bowman
 * Julian Stirling
 * Boyko Vodenicharski
 * Filip Ayazi
 * 
 * Much of the code is based on older code written by
 * James Sharkey and Fergus Riche
 * 
 * Released under GPL v3, 2021
 */

#include "main.h"
#include "config.h"
#include <Arduino.h>
#include <stdint.h>

#ifdef HELP
    #include "modules/help/help.h"
#endif

#ifdef STAGE
    #include "modules/stage/stage.h"
#endif

#ifdef ENDSTOPS
    #include "modules/endstops/endstops.h"
#endif

#ifdef LIGHT_SENSOR
    #include "modules/light_sensor/light_sensor.h"
#endif

#ifdef ADCS
    #include "modules/adcs/adcs.h"
#endif
const Command *registered_commands[MAX_COMMANDS];
void (*registered_loop_functions[MAX_MODULES])(void);

uint8_t registered_commands_count = 0;
uint8_t registered_loop_fn_count = 0;

const Command core_commands[] = {
    {"version", get_version},
    {"list_modules", get_modules},
    END_COMMAND};

void register_module(const Command commands[], void (*loop_fn)(void))
{
    for (int i = 0; CHECK_END_COMMAND(commands[i]); i++)
        registered_commands[registered_commands_count++] = &commands[i];

    if (loop_fn)
        registered_loop_functions[registered_loop_fn_count++] = loop_fn;
}

void handle_command(String received_command)
{
    for (int i = 0; CHECK_END_COMMAND(*registered_commands[i]); i++)
    {
        if (received_command.startsWith(registered_commands[i]->command))
            return registered_commands[i]->run(received_command.substring(strlen(registered_commands[i]->command)));
    }

#ifdef HELP
    Serial.println(F("Type 'help' for a list of commands."));
#else
    Serial.println(F("Invalid command"));
#endif
}

uint8_t parse_arguments(char **arguments, String command, uint8_t max_args)
{
    char buffer[MAX_ARGUMENT_LENGTH];

    uint8_t from = 0;
    while (command[from] == ' ' && from < command.length())
        from++;

    uint8_t parsed = 0;
    while (from < command.length() && parsed < max_args)
    {
        int space = command.indexOf(' ', from);
        if (space == -1)
            space = command.length();
        memcpy(buffer, &command[from], space - from);
        buffer[space - from] = '\0';
        arguments[parsed++] = strdup(buffer);
        from = space + 1;
    }

    return parsed;
}

void get_version(String)
{
    Serial.println(F(VERSION_STRING));
    return;
}

void get_modules(String)
{
#if defined(LIGHTSENSOR) && defined(LIGHT_SENSOR_ADAFRUIT_TSL2591)
    Serial.println(F("Light Sensor: TSL2591"));
#elif defined(LIGHTSENSOR) && defined(LIGHT_SENSOR_ADAFRUIT_ADS1115)
    Serial.println(F("Light Sensor: ADS1115"));
#endif

#if defined(ENDSTOPS)
    Serial.print("Endstops:");
    #ifdef ENDSTOPS_MIN
    Serial.print(" min");
    #endif
    #ifdef ENDSTOPS_MAX
    Serial.print(" max");
    #endif
    #ifdef ENDSTOPS_SOFT
    Serial.print(" soft");
    #endif
    Serial.println();
#endif
    Serial.println("--END--");
}

#ifndef UNIT_TEST
void setup()
{
    // initialise serial port
    Serial.begin(115200);
    while (!Serial)
        delay(1);

    register_module(core_commands, NULL);

    #ifdef HELP
    help_setup();
    #endif

    //register modules
    #ifdef STAGE
    stage_setup();
    #endif

    #ifdef LIGHT_SENSOR
    light_sensor_setup();
    #endif

    #ifdef ENDSTOPS
    endstops_setup();
    #endif

   #ifdef ADCS
    adcs_setup();
    #endif

    registered_commands[registered_commands_count] = &end_command;
    Serial.println(F(VERSION_STRING));
}

void loop()
{
    if (Serial.available())
    {
        handle_command(Serial.readStringUntil('\n'));
    }

    for (int i = 0; i < registered_loop_fn_count; i++)
        registered_loop_functions[i]();
}
#endif