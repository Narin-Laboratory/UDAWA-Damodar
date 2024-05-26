#include "main.h"

libudawaatmega328 nano;
Settings mySettings;
Sensors sensors;

OneWire oneWireDS18B20(mySettings.pinCels);
DallasTemperature ds18b20(&oneWireDS18B20);

void setup() {
  // put your setup code here, to run once:
  nano.begin();
  pinMode(mySettings.pinEC, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  nano.execute();

  StaticJsonDocument<DOCSIZE> doc;
  doc = nano.serialReadFromESP32();
  if(doc["err"] == nullptr && doc["err"].as<int>() != 1)
  {
    const char* method = doc["method"].as<const char*>();
    if(strcmp(method, (const char*) "setCfg") == 0)
    {
      // if(doc["example"] != nullptr){mySettings.example = doc["example"].as<int>();}
      if(doc["pinEC"] != nullptr){mySettings.pinEC = doc["pinEC"].as<int>();}
      if(doc["pinCels"] != nullptr){mySettings.pinCels = doc["pinCels"].as<int>();}
      if(doc["pinLoad"] != nullptr){mySettings.pinLoad = doc["pinLoad"].as<int>();}
      if(doc["pinFan"] != nullptr){mySettings.pinFan = doc["pinFan"].as<int>();}
      if(doc["VREF"] != nullptr){mySettings.VREF = doc["VREF"].as<float>();}
      if(doc["ECSamp"] != nullptr){mySettings.ECSamp = doc["ECSamp"].as<int>();}
    }
    else if(strcmp(method, (const char*) "readRawCels") == 0)
    {
      readRawCels();
      doc["celsRaw"] = sensors.celsRaw;
      nano.serialWriteToESP32(doc);
    }
    else if(strcmp(method, (const char*) "readRawEC") == 0)
    {
      readRawEC();
      doc["ecRaw"] = sensors.ecRaw;
      nano.serialWriteToESP32(doc);
    }

  }
  doc.clear();
}

float readRawCels(){
  ds18b20.requestTemperatures();
  sensors.celsRaw = ds18b20.getTempCByIndex(0);
  return sensors.celsRaw;
}

float readRawEC() {
  float rawADCAverageReading = 0;
  for (int i = 0; i < mySettings.ECSamp; i++) {
    rawADCAverageReading += analogRead(mySettings.pinEC);
    delay(5);
  }
  rawADCAverageReading /= mySettings.ECSamp;

  //float rawAdc = analogRead(mySettings.pinEC);
  float rawVoltage = rawADCAverageReading * (float)mySettings.VREF / 1024;
  
  float compensationCoefficient = 1.0+0.02*(sensors.celsRaw-25.0);
  float compensationVoltage = rawVoltage/compensationCoefficient;  //temperature compensation
  sensors.ecRaw = (133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
  //float voltageSquared = compensationVoltage * compensationVoltage;
  //float ecRawCoefficient = 133.42 * voltageSquared - 255.86 + 857.39 / compensationVoltage;
  //sensors.ecRaw = ecRawCoefficient * compensationVoltage * 0.5;

  
  return sensors.ecRaw;
}
