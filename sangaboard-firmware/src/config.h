#ifndef CONFIGURED
    //board choice
    #define BOARD_AUTO
    //#define BOARD_SANGABOARDv3
    //#define BOARD_SANGABOARDv2
    //#define BOARD_CUSTOM
    #include <boards.h> //defines wiring

    //general settings
    #define MAX_COMMANDS 50
    #define MAX_MODULES 10
    #define MAX_ARGUMENT_LENGTH 20 //used in argument parsing, takes up ram
    #define VERSION_STRING "Sangaboard Firmware v1.0-beta"
    #define DEBUG_ON

    //module choice
    #define HELP
    #define STAGE
    #define ADCS
    #define ENDSTOPS
    #define LIGHT_SENSOR

    //module configs
    #ifdef STAGE
        #define STAGE_N_MOTORS 3
    #endif
    #ifdef ADCS
        #define ADCS_N 3
    #endif

    #ifdef ENDSTOPS
        #define ENDSTOPS_MIN
        //#define ENDSTOPS_MAX
        #if defined(ENDSTOPS_MIN) || defined(ENDSTOPS_MAX)
            //endstops are closed when the pin is down  by default, this inverts the behaviour
            //soft endstops trigger when 0/MAX is reached in the direction without endstops
            #define ENDSTOPS_SOFT
        #endif
    #endif
    #ifdef LIGHT_SENSOR
    #define LIGHT_SENSOR_ADAFRUIT_TSL2591
    //#define LIGHT_SENSOR_ADAFRUIT_ADS1115
    #endif
    //config for non-standard platforms
    #ifndef MCU_PICO
        #define SUPPORT_EEPROM
    #endif
    #define CONFIGURED
#endif