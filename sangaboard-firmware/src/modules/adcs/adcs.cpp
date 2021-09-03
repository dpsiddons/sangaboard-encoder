#include "config.h"
#ifdef ADCS
#include "main.h"
#include<ADS1115_WE.h>
#include <Wire.h>
#include "adcs.h"

#define I2C_ADDRESS 0x48

extern const Command adc_commands[] = {
    {"read", adc_read},
    {"device", adc_device},
    {"device?", which_device}, 
    END_COMMAND};

int device=0;

void adcs_setup()
{
    register_module(adc_commands, NULL);
    ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);
    adc.setVoltageRange_mV(ADS1115_RANGE_4096);
    adc.setConvRate(ADS1115_128_SPS);
    adc.setMeasureMode(ADS1115_SINGLE);
}

//void adc_loop()
//{
  // read and print the analog in value:
//  adc_read(adcs[device]);
//}



void adc_read(){
    switch(device)
      case(0){
          float sensorValue = readChannel(ADS1115_COMP_0_GND);
          break;
      }
      case(1){
          float sensorValue = readChannel(ADS1115_COMP_1_GND);
          break;
      }
      case(2){
          float sensorValue = readChannel(ADS1115_COMP_2_GND);
          break;
      }
      case(3){
          float sensorValue = readChannel(ADS1115_COMP_3_GND);
          break;
      }
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