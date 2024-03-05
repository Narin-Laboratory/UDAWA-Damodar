#include <Arduino.h>
#include <libudawaatmega328.h>

struct Settings
{
    int pinTdsData = A0; // Pin data TDS
    int pinSuhuData = 4; // Pin data suhu
};

struct Sensors
{
   float ppm; // Pembacaan TDS yang difilter
   float cels; // Pembacaan suhu

   //parameter Kalman Filter
   float Q = 0.1; // Process noise covariance
   float R = 0.01; // Measurement noise covariance
   float P = 1.0; // Estimation error covariance
   float K; // Kalman gain
   float X = 0; // Filtered measurement value (initial guess)
};

void readSensors();
