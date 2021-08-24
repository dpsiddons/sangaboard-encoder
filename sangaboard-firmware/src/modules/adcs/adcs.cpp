#include "config.h"
#ifdef ADCS
#include "main.h"
#include <Wire.h>
#include "adcs.h"

extern const Command adc_commands[] = {
    {"read", adc_read},
    {"device", adc_device},
    {"device?", which_device}, 
    END_COMMAND};

int device=0;

void adcs_setup()
{
    register_module(adc_commands, NULL);
}

//void adc_loop()
//{
  // read and print the analog in value:
//  adc_read(adcs[device]);
//}



void adc_read(){
    int sensorValue = analogRead(adcs[device]);
    Serial.println(sensorValue, DEC);
}

void adc_device(String command){
    char *args[1];
    Serial.println("Parsing argument ");
    parse_arguments(args, command, 1);
    int number = atoi(args[1]);
    Serial.println("number # ");
    Serial.println(number);
    free(args[1]);
    device=number;
    Serial.println("Device # ");
    Serial.println(adcs[device], DEC); 
}

void which_device(){
    Serial.println(adcs[device], DEC);
}
#endif