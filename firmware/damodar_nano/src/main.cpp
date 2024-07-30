#include "main.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <GravityTDS.h>

libudawaatmega328 nano;
Settings mySettings;
Sensors sensors;

OneWire oneWireDS18B20(mySettings.pinSuhuData);
DallasTemperature ds18b20(&oneWireDS18B20);
GravityTDS gravityTDS;

void setup() {
  ds18b20.begin();
  gravityTDS.setPin(mySettings.pinTdsData);
  gravityTDS.setAref(5.0);
  gravityTDS.setAdcRange(1024);
  gravityTDS.begin();
  nano.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  nano.execute();

  StaticJsonDocument<DOCSIZE> doc;
  doc = nano.serialReadFromESP32();
  if(doc["err"] == nullptr && doc["err"].as<int>() != 1)
  {
    const char* method = doc["method"].as<const char*>();
    if(strcmp(method, (const char*) "setSettings") == 0)
    {
      // if(doc["example"] != nullptr){mySettings.example = doc["example"].as<int>();}
    }
    else if(strcmp(method, (const char*) "readSensors") == 0)
    {
      readSensors();
      doc["ppm"] = sensors.ppm;
      doc["cels"] = sensors.cels;
      nano.serialWriteToESP32(doc);
    }
  }
  doc.clear();
}
void readSensors() {
    //Baca suhu
    ds18b20.requestTemperatures();

    //Baca TdS dengan set nilai suhu
    gravityTDS.setTemperature(ds18b20.getTempCByIndex(0));
    gravityTDS.update();
    float tdsValue = gravityTDS.getTdsValue();

  //Kalman Filter untuk TDS
   sensors.P = sensors.P + sensors.Q;
   sensors.K = sensors.P / (sensors.P + sensors.R);
   sensors.X = sensors.X + sensors.K * (tdsValue - sensors.X);
   sensors.P = (1 - sensors.K) * sensors.P;


  // Menyesuaikan pembacaan TDS dengan faktor koreksi
    float correctionFactor = 345.0 / 396.0;
    sensors.ppm = sensors.X * correctionFactor; // Menyesuaikan nilai ppm dengan faktor koreksi
    sensors.cels = ds18b20.getTempCByIndex(0); //Nilai Suhu
}
