#include "config.h"
#ifdef LIGHT_SENSOR
#include "light_sensor.h"
#include "config.h"
#include "main.h"
#ifdef LIGHT_SENSOR_ADAFRUIT_TSL2591
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>
#endif
#ifdef LIGHT_SENSOR_ADAFRUIT_ADS1115
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#endif

const struct Command light_sensor_commands[] = {
    {"light_sensor_gain", light_sensor_gain},
    {"light_sensor_gain_values?", light_sensor_gain_values},
    {"light_sensor_integration_time?", light_sensor_integration_time},
    {"light_sensor_intensity", light_sensor_intensity},
    END_COMMAND};

void light_sensor_integration_time(String);

void light_sensor_setup()
{
    setup_light_sensor_device();
    register_module(light_sensor_commands, NULL);
}

void light_sensor_gain(String command)
{
    char *args[1];
    parse_arguments(args, command, 1);
    if (args[0][0] != '?')
    {
        int gain = atoi(args[0]);
        free(args[0]);
        return set_light_sensor_gain(gain);
    }
    free(args[0]);
    return print_light_sensor_gain();
}

#ifdef LIGHT_SENSOR_ADAFRUIT_TSL2591
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

void setup_light_sensor_device()
{
    if (tsl.begin())
    {
        tsl.setGain(TSL2591_GAIN_MED);
        tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS); // shortest integration time (bright light)
    }
    else
    {
        Serial.println(F("No light sensor found.  NB your board will start up faster if you recompile without light sensor support."));
    }
}

void print_light_sensor_gain()
{
    // Print the current gain value of the light sensor
    Serial.print(F("light sensor gain "));
    tsl2591Gain_t gain = tsl.getGain();
    switch (gain)
    {
    case TSL2591_GAIN_LOW:
        Serial.println(F("1x (Low)"));
        break;
    case TSL2591_GAIN_MED:
        Serial.println(F("25x (Medium)"));
        break;
    case TSL2591_GAIN_HIGH:
        Serial.println(F("428x (High)"));
        break;
    case TSL2591_GAIN_MAX:
        Serial.println(F("9876x (Max)"));
        break;
    }
}
void light_sensor_gain_values(String command)
{
    // Print the allowable gain values of the light sensor
    Serial.println(F("light sensor gains: 1x, 25x, 428x, 9876x"));
}

void set_light_sensor_gain(int newgain)
{
    // Set the current gain value of the light sensor, and print a confirmation.
    switch (newgain)
    {
    case 1:
        tsl.setGain(TSL2591_GAIN_LOW);
        break;
    case 25:
        tsl.setGain(TSL2591_GAIN_MED);
        break;
    case 428:
        tsl.setGain(TSL2591_GAIN_HIGH);
        break;
    case 9876:
        tsl.setGain(TSL2591_GAIN_MAX);
        break;
    default:
        Serial.println(F("Error: gain may only be 1, 25, 428, or 9876."));
        return;
    }
    print_light_sensor_gain();
}

void light_sensor_integration_time(String command)
{
    // Print the current integration time in milliseconds.
    Serial.print(F("light sensor integration time "));
    Serial.print((tsl.getTiming() + 1) * 100, DEC);
    Serial.println(F(" ms"));
}

void light_sensor_intensity(String command)
{
    // Print the current light value
    uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
    Serial.println(x, DEC);
}
#endif // ADAFRUIT_TSL2591

#ifdef LIGHT_SENSOR_ADAFRUIT_ADS1115
Adafruit_ADS1115 ads; // pass in a number for the sensor identifier (for your use later)

void setup_light_sensor_device()
{
    ads.begin();
    ads.setGain(GAIN_ONE);
}

void print_light_sensor_gain()
{
    // Print the current gain value of the light sensor
    Serial.print(F("light sensor gain "));
    adsGain_t gain = ads.getGain() M;
    switch (gain)
    {
    case GAIN_TWOTHIRDS:
        Serial.println(F("0.66x (specify 0)"));
        break;
    case GAIN_ONE:
        Serial.println(F("1x"));
        break;
    case GAIN_TWO:
        Serial.println(F("2x"));
        break;
    case GAIN_FOUR:
        Serial.println(F("4x"));
        break;
    case GAIN_EIGHT:
        Serial.println(F("8x"));
        break;
    case GAIN_SIXTEEN:
        Serial.println(F("16x"));
        break;
    }
}
void light_sensor_gain_values(String command)
{
    // Print the allowable gain values of the light sensor
    Serial.println(F("light sensor gains: 0.66x (specify 0), 1x, 2x, 4x, 8x, 16x"));
}

void set_light_sensor_gain(int newgain)
{
    // Set the current gain value of the light sensor, and print a confirmation.
    switch (newgain)
    {
    case 0:
        ads.setGain(GAIN_TWOTHIRDS);
        break;
    case 1:
        ads.setGain(GAIN_ONE);
        break;
    case 2:
        ads.setGain(GAIN_TWO);
        break;
    case 4:
        ads.setGain(GAIN_FOUR);
        break;
    case 8:
        ads.setGain(GAIN_EIGHT);
        break;
    case 16:
        ads.setGain(GAIN_SIXTEEN);
        break;
    default:
        Serial.println(F("Error: gain may only be 0, 1, 2, 4, 8, 16 (0 means 2/3)."));
        return;
    }
    print_light_sensor_gain();
}

void light_sensor_integration_time(String command)
{
    // Print the current integration time in milliseconds.
    Serial.println(F("integration time not supported for ADS1115"));
}

void light_sensor_intensity(String command)
{
    // Print the current light value
    // uint16_t x = ads.readADC_SingleEnded(0); //single ended measurement on pin 0
    uint16_t x = ads.readADC_Differential_0_1(); //differential measurement on pins 0,1
    Serial.println(x, DEC);
}
#endif // ADAFRUIT_ADS1115
#endif