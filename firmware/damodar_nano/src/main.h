#include <Arduino.h>
#include <libudawaatmega328.h>

struct Settings
{
    int pinTdsData = A0;
};

struct Sensors
{
   float ppm;
   float cels;
};


void readSensors();