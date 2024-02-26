#include <Arduino.h>
#include <libudawaatmega328.h>


struct Settings
{
    int pinTdsData = A0;
    int pinSuhuData = 4;
    
};

struct Sensors
{
   float ppm;
   float cels;
};

void readSensors();