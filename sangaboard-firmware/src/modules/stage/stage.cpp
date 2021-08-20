#include "stage.h"
#include "config.h"
#include <Arduino.h>
#include <limits.h>
#ifdef SUPPORT_EEPROM
#include <EEPROM.h>
#else
#include "dummyEEPROM.h"
#endif
#include "main.h"

// The array below has 3 stepper objects, for X,Y,Z respectively
const uint8_t n_motors = 3;
long min_step_delay = -1;
const uint8_t min_step_delay_eeprom = sizeof(long) * n_motors;
long ramp_time = -1;
const uint8_t ramp_time_eeprom = sizeof(long) * (n_motors + 1);
const uint8_t axis_max_eeprom = sizeof(long) * (n_motors + 2);
const uint8_t non_blocking_moves_eeprom = sizeof(long) * (n_motors + 3);
Stepper *motors[n_motors];
signed long current_pos[n_motors];
long steps_remaining[n_motors];

bool stage_moving = false;
bool notify_on_stop = false;
bool non_blocking_moves = false;

void stage_setup()
{
    // get the stepoper objects from the motor shield objects
    motors[0] = new Stepper(8, WIRING_MOTOR_X);
    motors[1] = new Stepper(8, WIRING_MOTOR_Y);
    motors[2] = new Stepper(8, WIRING_MOTOR_Z);
    EACH_MOTOR
    {
        motors[i]->setSpeed(10); // as a default set to 10 rpm, though this is ignored...
        steps_remaining[i] = 0;
        EEPROM.get(sizeof(long) * i, current_pos[i]); //read last saved position from EEPROM
                                                      //current_pos[i] = 0; //alternatively, reset on power cycle!
    }

    EEPROM.get(min_step_delay_eeprom, min_step_delay);
    if (min_step_delay < 0)
    { // -1 seems to be what we get if it's uninitialised.
        min_step_delay = 1000;
        EEPROM.put(min_step_delay_eeprom, min_step_delay);
    }
    EEPROM.get(ramp_time_eeprom, ramp_time);
    if (ramp_time < 0)
    { // -1 seems to be what we get if it's uninitialised.
        ramp_time = 0;
        EEPROM.put(ramp_time_eeprom, ramp_time);
    }

    //using non-blocking moves here to have blocking moves by default
    EEPROM.get(non_blocking_moves_eeprom, non_blocking_moves);

    register_module(stage_commands, stage_loop);
}

void stepMotor(int motor, long dx)
{
    //make a single step of a single motor.
    current_pos[motor] += dx;
    motors[motor]->stepMotor(((current_pos[motor] % 8) + 8) % 8); //forgive the double-modulo; I need 0-7 even for -ve numbers
}

void releaseMotor(int motor)
{
    //release the stepper motor (de-enegrise the coils)
    motors[motor]->stepMotor(8);
}

void print_position()
{
    EACH_MOTOR
    {
        if (i > 0)
            Serial.print(" ");
        Serial.print(current_pos[i]);
    }
    Serial.println();
}

unsigned long move_start_time = 0;
unsigned long distance_moved[n_motors];
long displacement[n_motors];
float final_scaled_t;
float step_delay[n_motors];
int8_t move_directions[n_motors];

void start_move(long displ[n_motors])
{
    // move all the axes in a nice move
    // split displacements into magnitude and direction, and find max. travel
    long max_steps = 0;
    EACH_MOTOR
    {
        move_directions[i] = displ[i] > 0 ? 1 : -1;
        displacement[i] = displ[i] * move_directions[i];
        if (displacement[i] > max_steps)
            max_steps = displacement[i];
    }
    // scale the step delays so the move goes in a straight line, with >=1 motor
    // running at max. speed.
    EACH_MOTOR
    {
        if (displacement[i] > 0)
        {
            step_delay[i] = float(max_steps) / float(displacement[i]) * float(min_step_delay);
        }
        else
        {
            step_delay[i] = 9999999999;
        }
    }
    EACH_MOTOR distance_moved[i] = 0;
    move_start_time = micros();
    final_scaled_t = (float)max_steps * min_step_delay; //NB total time taken will be final_scaled_t + 2*ramp_time
    stage_moving = true;
    if (!non_blocking_moves)
    {
        notify_on_stop = true;
    }
    else
    {
        Serial.println("done.");
    }
}

void stage_loop()
{
    if (!stage_moving)
        return;

    unsigned long now = micros();
    float elapsed_t;
    if (now < move_start_time) //overflow in micros() after ~70 min
        elapsed_t = (float)(now + (ULONG_MAX - move_start_time));
    else
        elapsed_t = (float)(now - move_start_time);

    float scaled_t; //scale time to allow for acceleration
    if (ramp_time > 0)
    {
        // if a ramp time is specified, accelerate at a constant acceleration for the
        // ramp time, then move at constant (maximum) speed, then decelerate.  If the
        // move is shorter than 2*ramp_time, accelerate then decelerate.
        float remaining_t = final_scaled_t + ramp_time - elapsed_t;
        if (elapsed_t < ramp_time && remaining_t > elapsed_t)
        { //for the first ramp_time, gradually accelerate
            scaled_t = elapsed_t * elapsed_t / (2 * ramp_time);
        }
        else if (remaining_t < ramp_time)
        {
            scaled_t = final_scaled_t - remaining_t * remaining_t / (2 * ramp_time);
        }
        else
        {
            scaled_t = elapsed_t - ramp_time / 2;
        }
    }
    else
    {
        scaled_t = elapsed_t;
    }

    stage_moving = false;
    EACH_MOTOR
    {
        if ((long)distance_moved[i] < displacement[i])
        {
            stage_moving = true; //only if all axes are done are we truly finished.

            // check if it's time to take another step and move if needed.
            if (scaled_t > ((float)distance_moved[i] + 0.5) * step_delay[i])
            {
                stepMotor(i, move_directions[i]);
                distance_moved[i]++;
            }
        }
    }

    if (!stage_moving && notify_on_stop)
    {
        Serial.println("stopped");
        notify_on_stop = false;
    }
}

void stage_move_single_axis(uint8_t axis, String command)
{
    char *args[1];
    parse_arguments(args, command, 1);
    int move = atoi(args[0]);
    EACH_MOTOR displacement[i] = 0;
    displacement[axis] = move;
    start_move(displacement);
    free(args[0]);
}

void stage_mrx(String command)
{
    stage_move_single_axis(0, command);
}

void stage_mry(String command)
{
    stage_move_single_axis(1, command);
}

void stage_mrz(String command)
{
    stage_move_single_axis(2, command);
}

void stage_mr(String command)
{
    char *args[3];
    parse_arguments(args, command, 3);
    EACH_MOTOR
    {
        displacement[i] = atol(args[i]);
        free(args[i]);
    }

    start_move(displacement);
}

void stage_release(String command)
{
    EACH_MOTOR
    {
        releaseMotor(i);
    }
    Serial.println("done");
}

void stage_p(String command)
{
    print_position();
}

void stage_min_step_delay(String command)
{
    char *args[1];
    parse_arguments(args, command, 1);
    if (args[0][0] == '?')
    {
        Serial.print("minimum step delay ");
        Serial.println(min_step_delay);
    }
    else
    {
        min_step_delay = atol(args[0]);
        EEPROM.put(min_step_delay_eeprom, min_step_delay);
        Serial.println("done.");
    }
    free(args[0]);
}

void stage_ramp_time(String command)
{
    char *args[1];
    parse_arguments(args, command, 1);
    if (args[0][0] == '?')
    {
        Serial.print("ramp_time ");
        Serial.println(ramp_time);
    }
    else
    {
        ramp_time = atol(args[0]);
        EEPROM.put(ramp_time_eeprom, ramp_time);
        Serial.println("done.");
    }
    free(args[0]);
}

void update_blocking_moves(String command)
{
    char *args[1];
    parse_arguments(args, command, 1);
    if (args[0][0] == '?')
    {
        Serial.print("blocking_moves ");
        Serial.println(non_blocking_moves ? "false" : "true");
    }
    else
    {
        non_blocking_moves = !(args[0][0] == 't');
        EEPROM.put(non_blocking_moves_eeprom, non_blocking_moves);
        Serial.println("done.");
    }
}

void stage_zero(String command)
{
    EACH_MOTOR current_pos[i] = 0;
    Serial.println(F("position reset to 0 0 0"));
    EEPROM.put(0, current_pos);
}

void stage_stop(String command)
{
    stage_moving = false;
    Serial.println("Move aborted");
}

void is_stage_moving(String command)
{
    //TODO: should I bother checking for a ?
    Serial.println(stage_moving ? "true" : "false");
}

void activate_notify_on_stop(String command)
{
    if (!stage_moving)
    {
        Serial.println("Error: stage is not moving");
    }
    else
    {
        notify_on_stop = true;
    }
}

//TODO: move help strings to program memory
//F() cannot be used outside block context though
extern const Command stage_commands[] = {
    {"mrx", stage_mrx},
    {"mry", stage_mry},
    {"mrz", stage_mrz},
    {"mr", stage_mr},
    {"release", stage_release},
    {"p", stage_p},
    {"ramp_time", stage_ramp_time},
    {"min_step_delay", stage_min_step_delay},
    {"dt", stage_min_step_delay},
    {"zero", stage_zero},
    {"stop", stage_stop},
    {"moving", is_stage_moving},
    {"notify_on_stop", activate_notify_on_stop},
    {"blocking_moves", update_blocking_moves},
    END_COMMAND};