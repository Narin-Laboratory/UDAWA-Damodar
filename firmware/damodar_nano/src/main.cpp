#include "main.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

libudawaatmega328 nano;
Settings mySettings;
Sensors sensors;

OneWire oneWireDS18B20(mySettings.pinSuhuData);
DallasTemperature ds18b20(&oneWireDS18B20);



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
  // Baca suhu
  ds18b20.requestTemperatures();
  sensors.cels = ds18b20.getTempCByIndex(0);
  
  // Baca dan filter pembacaan TDS
  int analogValue = analogRead(mySettings.pinTdsData);
  float voltage = analogValue * (5.0 / 1023.0);
  float tdsRawValue = (voltage - 0.0) * 314.34; //koveksi tegangan ke ppm
  
  
  // Update filter Kalman
  sensors.P = sensors.P + sensors.Q;
  sensors.K = sensors.P / (sensors.P + sensors.R);
  sensors.X = sensors.X + sensors.K * (tdsRawValue - sensors.X);
  sensors.P = (1 - sensors.K) * sensors.P;
  sensors.ppm = sensors.X;
}
