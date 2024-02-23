#include "main.h"

libudawaatmega328 nano;

Settings mySettings;

Sensors sensors;

void setup() {
  // put your setup code here, to run once:
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
      if(doc["example"] != nullptr){mySettings.example = doc["example"].as<int>();}
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

void readSensors(){
  sensors.ppm = rand();
  sensors.cels = rand();

  mySettings.pinTdsData;
}