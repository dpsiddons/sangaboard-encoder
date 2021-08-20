#include "config.h"
#ifdef ENDSTOPS
#include "endstops.h"
#include "../stage/stage.h"
#ifdef SUPPORT_EEPROM
    #include <EEPROM.h>
#else
    #include "dummyEEPROM.h"
#endif

unsigned long retreat_steps = 5000;
unsigned long home_move_steps = 100000;
unsigned long axis_max[STAGE_N_MOTORS];
const uint8_t axis_max_eeprom = sizeof(long)*(STAGE_N_MOTORS+2);
#ifdef ENDSTOPS_MIN
const uint8_t endstops_min_pins[] = WIRING_ENDSTOPS_MIN;
#endif
#ifdef ENDSTOPS_MAX
const uint8_t endstops_max_pins[] = WIRING_ENDSTOPS_MAX;
#endif
bool endstops_enabled = true;

//we want to be able to abort homing, so all calls are non-blocking
//we start with a very long move towards the endstops
//when this is finished we start a move away from the endstops
//when this is finished we set a very slow speed and start a move
//back towards the endstops, after which we move away a few thousands
//steps (the switch has some hysteresis)
bool homing_initial_move = false;
bool homing_moving_away = false;
bool homing_final = false;
uint8_t homing_axes = 0;
int8_t homing_direction = 0;
long previous_step_delay = 0;

boolean endstop_triggered(uint8_t axis, int8_t direction)
{
#if defined(ENDSTOPS_MIN)
    if (direction < 0)
    {
        int value = analogRead(endstops_min_pins[axis]);
        if (ENDSTOPS_INVERT != (value < 100))
            return true;
    }
#elif defined(ENDSTOPS_SOFT)
    if (direction < 0 && current_pos[axis] < 1)
        return true;
#endif

#if defined(ENDSTOPS_MAX)
    if (direction > 0)
    {
        int value = analogRead(endstops_max_pins[axis]);
        if (ENDSTOPS_INVERT != (value < 100))
            return true;
    }
#elif defined(ENDSTOPS_SOFT)
    if (direction > 0 && current_pos[axis] >= (signed long) axis_max[axis])
        return true;
#endif
    return false;
}

void endstops_loop()
{
    if (!endstops_enabled)
        return;

    endstops_check();

    if (homing_initial_move)
    {
        if (stage_moving)
            return;
        D(F("Homing initial move complete"));
        homing_initial_move = false;
        homing_moving_away = true;
        home_retreat();
        return;
    }

    if (homing_moving_away)
    {
        if (stage_moving)
            return;
        D(F("Homing retreat complete"));

        homing_moving_away = false;
        homing_final = true;
        home_final();
        return;
    }

    if (homing_final)
    {
        if (stage_moving)
            return;
        D(F("Homing complete complete"));
        homing_final = false;
        min_step_delay = previous_step_delay;
        home_retreat();
        Serial.println("done.");
        return;
    }
}

int8_t endstops_check()
{
    int8_t endstop_break = 0;
#if defined(ENDSTOPS_MIN) || defined(ENDSTOPS_MAX)
    EACH_MOTOR
    {
        //TODO: why the second part? isn't move direction enough?
        if (endstop_triggered(i, move_directions[i]))
        {
            endstop_break = move_directions[i] * (i + 1);
            D(endstop_break);
            D(move_directions[i]);
        }
    }

    if (endstop_break != 0 && stage_moving)
    {
        Serial.print(F("Endstop hit:"));
        Serial.println(endstop_break);
        //if we have both min/max endstops, axis_max is adjusted to the correct value
        //if we only have min, we go from 0 -> predefined axis_max
        //if we only have max, we go from 0 -> predefined axis_max
        //the predefined axis_max are the travel distances in steps
        if (endstop_break < 0)
        {
            current_pos[-endstop_break - 1] = 0;
            //abort move in this direction
            displacement[-endstop_break - 1] = 0;
        }
        else
        {
            displacement[endstop_break - 1] = 0;
#if defined(ENDSTOPS_MIN) && defined(ENDSTOPS_MAX)
            axis_max[endstop_break - 1] = current_pos[endstop_break - 1];
#elif defined(ENDSTOP_MAX) //we do not do this for ENDSTOPS_SOFT
            current_pos[endstop_break - 1] = axis_max[endstop_break - 1];
#else
            ;
        }
#endif
    }
#endif //ENDSTOPS_MIN || ENDSTOPS_MAX
    return endstop_break;
}

void endstops_status(String command)
{
    EACH_MOTOR
    {
        if (i > 0)
            Serial.print(" ");
        if (endstop_triggered(i, -1))
            Serial.print("-1");
        else if (endstop_triggered(i, 1))
            Serial.print("1");
        else
            Serial.print("0");
    }

    Serial.println();
}

void print_axes_max(String command)
{
    EACH_MOTOR
    {
        if (i > 0)
            Serial.print(" ");
        Serial.print(axis_max[i]);
    }
    Serial.println();
}

void home_start(uint8_t axes, int8_t direction)
{
    homing_axes = axes;
    homing_direction = direction;
    homing_initial_move = true;
    long displacement[3];
    EACH_MOTOR
    {
        if (((homing_axes >> i) & 1) == 1)
            displacement[i] = homing_direction * home_move_steps;
        else
            displacement[i] = 0;
    }

    start_move(displacement);
}

void home_retreat()
{
    long displacement[3];
    EACH_MOTOR
    {
        if (((homing_axes >> i) & 1) == 1)
            displacement[i] = -homing_direction * retreat_steps;
        else
            displacement[i] = 0;
    }
    start_move(displacement);
}

void home_final()
{
    long displacement[3];
    EACH_MOTOR
    {
        if (((homing_axes >> i) & 1) == 1)
            displacement[i] = 2 * homing_direction * retreat_steps;
        else
            displacement[i] = 0;
    }

    previous_step_delay = min_step_delay;
    min_step_delay = 4000;
    start_move(displacement);
}

void endstops_home_min(String command)
{
#ifndef ENDSTOPS_MIN
    Serial.println(F("Min endstops not installed"));
    return;
#endif
    if (command.equals("home_min"))
        return home_start(7, -1);
    char * arg[1];
    parse_arguments(arg, command, 1);
    uint8_t axes = atoi(arg[0]);
    free(arg[0]);
    home_start(axes, -1);
}

void endstops_home_max(String command)
{
#ifndef ENDSTOPS_MAX
    Serial.println(F("Max endstops not installed"));
    return;
#endif
    if (command.equals("home_max"))
        return home_start(7, 1);
    char * arg[1];
    parse_arguments(arg, command, 1);
    uint8_t axes = atoi(arg[0]);
    free(arg[0]);
    home_start(axes, 1);
}

void endstops_set_max(String command)
{
    char * args[3];
    parse_arguments(args, command, 3);
    EACH_MOTOR
    {
        axis_max[i] = atol(args[i]);
        free(args[i]);
    }
    EEPROM.put(axis_max_eeprom, axis_max);

}

void endstops_setup()
{
    #ifdef ENDSTOPS_MIN
    EACH_MOTOR{
      if(ENDSTOPS_PULLUPS)
        pinMode(endstops_min_pins[i], INPUT_PULLUP);
      else
        pinMode(endstops_min_pins[i], INPUT);
    }
    #endif

    #ifdef ENDSTOPS_MAX
    EACH_MOTOR{
      if(ENDSTOPS_PULLUPS)
        pinMode(endstops_max_pins[i], INPUT_PULLUP);
      else
        pinMode(endstops_max_pins[i], INPUT);
    }
    #endif
    EACH_MOTOR
    {
        EEPROM.get(axis_max_eeprom+i*sizeof(long), axis_max[i]);
    }
    register_module(endstops_commands, endstops_loop);
}

//endstops? - get triggered endstops in (1,0,-1) format for max, none, min"));
//      Serial.println(F("home_min <axes?>               - home given (00000zyx byte, e.g. 1 for x) or all axes to their min position"));
//      Serial.println(F("home_max <axes?>               - home given (00000zyx byte, e.g. 3 for x and y) or all axes to their max position"));
//      Serial.println(F("max_p?                         - return positions of max endstops"));
//      Serial.println(F("max <d> <d> <d>
extern const Command endstops_commands[] = {
    {"endstops?", endstops_status},
    {"home_min", endstops_home_min},
    {"home_max", endstops_home_max},
    {"max_p?", print_axes_max},
    {"max", endstops_set_max},
    END_COMMAND};
#endif