#include "config.h"
#ifdef HELP
#include "help.h"

void help_setup()
{
    register_module(help_commands, NULL);
}

void help(String command)
{
    Serial.println("");
    Serial.print("Board: ");
    Serial.println(F(BOARD_STRING));

#ifdef LIGHT_SENSOR
#if defined ADAFRUIT_TSL2591
    Serial.println(F("Compiled with Adafruit TSL2591 support"));
#elif defined ADAFRUIT_ADS1115
    Serial.println(F("Compiled with Adafruit ADS1115 support"));
#endif
#endif //LIGHT_SENSOR


#ifdef ENDSTOPS
#ifdef ENDSTOPS_MIN
    Serial.println(F("Compiled with min endstops support"));
#endif
#ifdef ENDSTOPS_MAX
    Serial.println(F("Compiled with max endstops support"));
#endif
#endif //ENDSTOPS


    Serial.println("");
    Serial.println(F("Commands (terminated by a newline character):"));
#ifdef STAGE
    Serial.println(F("mrx <d>                        - relative move in x"));
    Serial.println(F("mry <d>                        - relative move in y"));
    Serial.println(F("mrz <d>                        - relative move in z"));
    Serial.println(F("mr <d> <d> <d>                 - relative move in all 3 axes"));
    Serial.println(F("release                        - de-energise all motors"));
    Serial.println(F("p?                             - print position (3 space-separated integers"));
    Serial.println(F("ramp_time <d>                  - set the time taken to accelerate/decelerate in us"));
    Serial.println(F("min_step_delay <d>             - set the minimum time between steps in us."));
    Serial.println(F("dt <d>                         - set the minimum time between steps in us."));
    Serial.println(F("ramp_time?                     - get the time taken to accelerate/decelerate in us"));
    Serial.println(F("min_step_delay?                - get the minimum time between steps in us."));
    Serial.println(F("zero                           - set the current position to zero."));
    Serial.println(F("stop                           - stop a move in progress."));
    Serial.println(F("blocking_moves?                - get blocking moves enabled"));
    Serial.println(F("blocking_moves <bool>          - enable/disable blocking moves"));
    Serial.println(F("moving?                        - check if a move is in progress"));
    Serial.println(F("notify_on_stop                 - respond with stopped when current move finishes"));
#endif //STAGE

#ifdef LIGHT_SENSOR
    Serial.println(F("light_sensor_gain <d>          - set the gain of the light sensor"));
    Serial.println(F("light_sensor_gain?             - get the gain of the light sensor"));
    Serial.println(F("light_sensor_gain_values?      - get the allowable gain values of the light sensor"));
    Serial.println(F("light_sensor_integration_time? - get the integration time in milliseconds"));
    Serial.println(F("light_sensor_intensity?        - read the current value from the full spectrum diode"));
#endif //LIGHT_SENSOR

#if defined(ENDSTOPS_MIN) || defined(ENDSTOPS_MAX)
    Serial.println(F("endstops?                      - get triggered endstops in (1,0,-1) format for max, none, min"));
    Serial.println(F("home_min <axes?>               - home given (00000zyx byte, e.g. 1 for x) or all axes to their min position"));
    Serial.println(F("home_max <axes?>               - home given (00000zyx byte, e.g. 3 for x and y) or all axes to their max position"));
    Serial.println(F("max_p?                         - return positions of max endstops"));
    Serial.println(F("max <d> <d> <d>                - set maximum positions"));
#endif
#if defined(ADCS)
    Serial.println(F("device <d>                     - set which encoder channel to read"));
    Serial.println(F("device?                        - show which encoder channel to read"));
    Serial.println(F("read                           - read encoder channel"));
#endif

    //Serial.println(F("test_mode <s>                  - set test_mode <on> <off>"));
    Serial.println(F("version                        - get firmware version string"));
    Serial.println("");
    Serial.println("Input Key:");
    Serial.println(F("<d>                            - a decimal integer."));
    Serial.println("");
    Serial.println("--END--");
}
#endif //HELP