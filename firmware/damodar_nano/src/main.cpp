#include "main.h"
#include <OneWire.h>
#include <DallasTemperature.h>


libudawaatmega328 nano;
Settings mySettings;
Sensors sensors;

OneWire oneWireDS18B20(mySettings.pinSuhuData);
DallasTemperature ds18b20(&oneWireDS18B20);

// Variabel-variabel untuk Filter Kalman
float estimasi_ppm = 0; // Estimasi nilai ppm oleh filter Kalman
float error_estimasi = 1; // Error estimasi (P)
float error_pengukuran = 3; // Error pengukuran (R), asumsikan nilai ini atau sesuaikan berdasarkan kebisingan sensor
float kalman_gain = 0; // Kalman gain (K) // Sensor noise. Adjust this value based on your sensor's performance.

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

void readSensors(){
  
      // Membaca nilai sensor TDS
    int sensorValueTds = analogRead(mySettings.pinTdsData);

    // Memperbarui estimasi dengan prediksi sebelumnya
    float prediksi_ppm = estimasi_ppm; // Dalam kasus sederhana ini, modelnya adalah identitas

    // Menghitung Kalman Gain
    kalman_gain = error_estimasi / (error_estimasi + error_pengukuran);

    // Memperbarui estimasi dengan pengukuran terkini
    estimasi_ppm = prediksi_ppm + kalman_gain * (sensorValueTds - prediksi_ppm);

    // Memperbarui error estimasi
    error_estimasi = (1 - kalman_gain) * error_estimasi;

    // Simpan nilai ppm yang difilter ke dalam struktur Sensors
    sensors.ppm = estimasi_ppm;

    ds18b20.requestTemperatures();  // Mengirim perintah untuk membaca suhu
    float temperatureCelsius = ds18b20.getTempCByIndex(0);  // Mengambil nilai suhu dalam derajat Celsius

    // Menyimpan nilai suhu ke dalam struktur Sensors
    sensors.cels = temperatureCelsius;
}
