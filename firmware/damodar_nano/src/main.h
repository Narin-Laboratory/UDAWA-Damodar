#include <Arduino.h>
#include <libudawaatmega328.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#define EC_SAMPLING_COUNT 50

struct Settings
{
    int pinEC = A0; 
    int pinCels = 4; 
    int pinLoad = 9;
    int pinFan = 10;

    float VREF = 5.0;  

    int ECSamp = 50;   
};

struct Sensors
{
   float ecRaw;
   float celsRaw;
};

float readRawCels();
float readRawEC();
