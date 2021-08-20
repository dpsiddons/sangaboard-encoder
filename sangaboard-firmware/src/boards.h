#ifdef BOARD_AUTO
    #ifdef MCU_LEONARDO
        #define BOARD_SANGABOARDv3
    #elif defined(MCU_NANO)
        #define BOARD_SANGABOARDv2
    #elif defined(MCU_BLUEPILL)
        #define BOARD_CUSTOM_BLUEPILL
    #elif defined(MCU_PICO)
        #define BOARD_CUSTOM
    #else
        #error "Invalid build config - must specify MCU type"
    #endif
#endif

#if defined(BOARD_SANGABOARDv3)
    #define BOARD_STRING "Sangaboard v0.3"
    #define WIRING_MOTOR_X 8, 9, 10, 11
    #define WIRING_MOTOR_Y 5, 13, 4, 12
    #define WIRING_MOTOR_Z 6, 7, A5, A4
    #define ENDSTOPS_INVERT false
    #define ENDSTOPS_PULLUPS true
    #define WIRING_ENDSTOPS_MIN {A0,A1,A2}
    #define WIRING_ENDSTOPS_MAX {A3,A4,A5}
#elif defined(BOARD_SANGABOARDv2)
    #define BOARD_STRING "Sangaboard v0.2"
    #define WIRING_MOTOR_X 13, 12, 11, 10
    #define WIRING_MOTOR_Y 9, 8, 7, 6
    #define WIRING_MOTOR_Z 5, 4, 3, 2
    #define ENDSTOPS_INVERT false
    #define ENDSTOPS_PULLUPS true
    #define WIRING_ENDSTOPS_MIN {A0,A1,A2}
    #define WIRING_ENDSTOPS_MAX {A3,A4,A5}
#elif defined(BOARD_CUSTOM_BLUEPILL)
    #define BOARD_STRING "Custom Bluepill board"
    #define WIRING_MOTOR_X PB11, PB10, PB1, PB0
    #define WIRING_MOTOR_Y PA7, PA6, PA5, PA4
    #define WIRING_MOTOR_Z PA3, PA2, PA1, PA0
    #define ENDSTOPS_INVERT false
    #define ENDSTOPS_PULLUPS true
    #define WIRING_ENDSTOPS_MIN {A0,A1,A2}
    #define WIRING_ENDSTOPS_MAX {A3,A4,A5}
#else
    #define BOARD_STRING "Custom board"
    #define WIRING_MOTOR_X 13, 12, 11, 10
    #define WIRING_MOTOR_Y 9, 8, 7, 6
    #define WIRING_MOTOR_Z 5, 4, 3, 2
    #define ENDSTOPS_INVERT false
    #define ENDSTOPS_PULLUPS true
    #define WIRING_ENDSTOPS_MIN {A0,A1,A2}
    #define WIRING_ENDSTOPS_MAX {A3,A4,A5}
#endif