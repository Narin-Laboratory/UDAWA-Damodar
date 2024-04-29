/**
 * UDAWA - Universal Digital Agriculture Watering Assistant
 * Firmware for Damodar UDAWA Board (Fertigation Water Monitoring)
 * Licensed under aGPLv3
 * Researched and developed by PRITA Research Group & Narin Laboratory
 * prita.undiknas.ac.id | narin.co.id
**/

#ifndef main_h
#define main_h
#include <Arduino.h>
static const char* CA_CERT PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIGSjCCBDKgAwIBAgIJAMxU3KljbiooMA0GCSqGSIb3DQEBCwUAMIGwMQswCQYD
VQQGEwJJRDENMAsGA1UECAwEQmFsaTEQMA4GA1UEBwwHR2lhbnlhcjEhMB8GA1UE
CgwYQ1YuIE5hcmF5YW5hIEluc3RydW1lbnRzMSQwIgYDVQQLDBtOYXJpbiBDZXJ0
aWZpY2F0ZSBBdXRob3JpdHkxFjAUBgNVBAMMDU5hcmluIFJvb3QgQ0ExHzAdBgkq
hkiG9w0BCQEWEGNlcnRAbmFyaW4uY28uaWQwIBcNMjAwMTE2MDUyMjM1WhgPMjA1
MDAxMDgwNTIyMzVaMIGwMQswCQYDVQQGEwJJRDENMAsGA1UECAwEQmFsaTEQMA4G
A1UEBwwHR2lhbnlhcjEhMB8GA1UECgwYQ1YuIE5hcmF5YW5hIEluc3RydW1lbnRz
MSQwIgYDVQQLDBtOYXJpbiBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkxFjAUBgNVBAMM
DU5hcmluIFJvb3QgQ0ExHzAdBgkqhkiG9w0BCQEWEGNlcnRAbmFyaW4uY28uaWQw
ggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQC7SU2ahwCe1KktoaUEQLmr
E91S2UwaqGvcGksy9j08GnI1NU1MpqsVrPSxuLQRr7ww2IG9hzKN0rKIhkXUfBCJ
X8/K7bxEkLl2yfcJjql90/EdAjWClo3CURM1zIqzxggkZKmdEGrPV/WGkYchmxuT
QvYDoPVLScXhN7NtTfzd3x/zWwe4WHg4THpfqeyE6vCLoNeDKvF2GsP0xsYtows8
pTjKH9gh0kFi+aYoVxjbH8KB78ktWAOo1T2db3KUF4/gYweYk4b/vS1egdDm821/
6qC7XrsnaApyRm73RKtmhAzldx9D1YqdVbFIx5oRlEg3+uI7hv/YD6Icfhazw1ql
Su8U7g8Ax8OPVdjdJ41lgkFs+OpY4GnfzjIzhvQ+kRIPyDQQaLwCxFZJZIa2jAnB
R5GzSM+Py0d+oStELotd0O3kLC7z4eFdxfRuaaQzofn/aUT1K7NsbG8V7rC3lG9P
8Jc+SU7zP/XSqVjKFTzRnIQ6C4WwkdWwS7uN2FSQBmlIw4EHaYcoMbQCVN2DcGFE
iMH+8/kp99UBKCu2MKq1zM+W0n+dNJ65EdeygOw580EIdNY5DPbpQTeNMt1idXF2
8C9jMyInGt6ZgZ5IfjExfDDIb6WYl3KRmZxqXjWCMg1e5TwMrbeoeg3R6kFY3lG4
TReG7Zjp1PQ3vxMC4ZWcpQIDAQABo2MwYTAdBgNVHQ4EFgQU4rkKARnad9D4bdWs
t3Jo8KKTxHAwHwYDVR0jBBgwFoAU4rkKARnad9D4bdWst3Jo8KKTxHAwDwYDVR0T
AQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAYYwDQYJKoZIhvcNAQELBQADggIBAJAY
pl1fdOG8GpvTIDts/H1CZbT6bF85+FnAV6abL/X4dIx/sQpg9TASAicyMzqUtDSb
2WaD3tkDvFhu7/vzG62x0tcpGw99Bxy0pkSkO9J3KrrxxiK21810aZnxOLZFuvgJ
2O/jugBK8MdCemBpmX93imgXSLJSXJf5/yVcETXFGUmf5p7Ze3Wdi0AxFvjPe2yN
D2MFvp2dtJ9mFaGaCG9v5wjyVVZM+oTGI9JzfNURq//qYWX+Tz9HJVNVeuYvEUnb
LXe9FVZej1+RVsBut9eCyo4GWOqMgRWp/dyMKz3shHFec0pc0fluo7yQH82OonoM
ZzkqgKVmkP5LVW0WqrDKbPTmpsq3ISYJwe7Msnu5D47iUnuc22axPzOH7ZRXE+2n
1Vkig4iYxz2IFZCwO3Ei9LxDlaJh+juHNnS0ziosDrTw0c/VWjkV+XwhRhfNq+Cx
crjMwThsIrz+JXrTihppMvSQhJHjIB/KoiUsa63qVsv6JA+yeBwvthwJ4Kl2ioDg
rH2VNKMU9e/dsWRqfRdUxH29pyJHQFjlv8MXWlFrKgoyrOLN2wkO2RJdKm0hblZW
vh+RH1AiwshFKw9rxdUXJBGGVgn5F0Ie4alDI8ehelOpmrZgFYMOCpcFSpJ5vbXM
ny6l9/duT2POAsUN5IwHGDu8b2NT+vCUQRFVHY31
-----END CERTIFICATE-----
)EOF";

#define CURRENT_FIRMWARE_TITLE "Damodar"
#define CURRENT_FIRMWARE_VERSION "0.0.1"
#define DOCSIZE 1024
#define DOCSIZE_MIN 512
#define DOCSIZE_SETTINGS 2048
#define USE_SERIAL2
#define USE_WEB_IFACE
#define USE_ASYNC_WEB
#define USE_INTERNAL_UI
#define USE_HW_RTC
#define USE_WIFI_OTA
//#define USE_WIFI_LOGGER
//#define USE_SDCARD_LOG
#define USE_SPIFFS_LOG
#define USE_DISK_LOG
#define STACKSIZE_WIFIKEEPER 3000
#define STACKSIZE_SETALARM 3700
#define STACKSIZE_WIFIOTA 4096
#define STACKSIZE_TB 6000
#define STACKSIZE_IFACE 3000
#define STACKSIZE_PUBLISHDEVTEL 4500 //6000
#define STACKSIZE_WSSENDTELEMETRY 4500 //6000
#define STACKSIZE_SENSORS 4500


#include <libudawa.h>
#include <TimeLib.h>
#include <Statistical.h>

using namespace libudawa;
const char* settingsPath = PSTR("/settings.json");
struct Settings
{
    uint16_t itDt = 60000;
    uint16_t itDa = 10000;
    uint16_t itDr = 1000;

    uint16_t itSt = 60000;
    uint16_t itSa = 5000;
    uint16_t itSr = 1000;

    uint8_t s1tx = 33; //V3.1 33, V3 32
    uint8_t s1rx = 32; //V3.1 32, V3 4

    int pinTDS = 14; 
    int pinCels = 4; 
    int pinLoad = 9;
    int pinFan = 10;

    float VREF = 5.0;  

    /*
    x_est represents the current state estimation.
    P_est represents the current estimation error covariance.
    Q is the process noise covariance, representing the uncertainty in the process dynamics.
    R is the measurement noise covariance, representing the uncertainty in the sensor measurements.
    */

    float tdsXEst = 100.0;
    float tdsPEst = 1;
    float tdsQ = 0.01;     
    float tdsR = 1.0; 
    float tdsK = 0.0;
    float tdsPTmp = 0.0;
    float tdsXTmp = 0.0;
    int TDSSamp = 50;

    float celsXEst = 25.0;
    float celsPEst = 1;
    float celsQ = 0.01;     
    float celsR = 1.0; 
    float celsK = 0.0;
    float celsPTmp = 0.0;
    float celsXTmp = 0.0;
    
};

struct States
{
    bool flag_sensors = false;
};
States myStates;

#ifdef USE_WEB_IFACE
struct WSPayloadSensors
{
    float ppm;
    float ppmRaw;
    float ppmAvg;
    float ppmMax;
    float ppmMin;
    float ec;
   
    float cels;
    float celsRaw;
    float celsAvg;
    float celsMax;
    float celsMin;

    float volt;
    float voltRaw;
    float voltMin;
    float voltMax;
    float voltAvg;

    float amp;
    float ampRaw;
    float ampMin;
    float ampMax;
    float ampAvg;

    float watt;
    float wattRaw;
    float wattMin;
    float wattMax;
    float wattAvg;

    unsigned long ts;
};
QueueHandle_t xQueueWsPayloadSensors;
#endif

Settings mySettings;
WSPayloadSensors sensors;

BaseType_t xReturnedWsSendTelemetry;
BaseType_t xReturnedPublishDevTel;
BaseType_t xReturnedSensors;

TaskHandle_t xHandleWsSendTelemetry = NULL;
TaskHandle_t xHandlePublishDevTel = NULL;
TaskHandle_t xHandleSensors = NULL;

SemaphoreHandle_t xSemaphoreSensors = NULL;


float calcKalmanFilter(float raw, float &Q, float &R, float &x_est, float &P_est, float &K, float &P_temp, float &x_temp);
void loadSettings();
void saveSettings();
void attUpdateCb(const Shared_Attribute_Data &data);
void onTbConnected();
void onTbDisconnected();
RPC_Response genericClientRPC(const RPC_Data &data);
void onReboot();
void onAlarm(int code);
void onSyncClientAttr(uint8_t direction);
#ifdef USE_WEB_IFACE
void onWsEvent(const JsonObject &data);
void wsSendTelemetryTR(void *arg);
#endif
void publishDeviceTelemetryTR(void * arg);
void onMQTTUpdateStart();
void onMQTTUpdateEnd();
void setPanic(const RPC_Data &data);
void sensorsTR(void *arg);
void RPCRequestAIAnalyzerProc(const JsonVariantConst &data);
const RPC_Request_Callback RPCRequestAIAnalyzerCb(PSTR("AIAnalyzer"), &RPCRequestAIAnalyzerProc);
bool RPCRequestAIAnalyzer();

/**
 * @brief UDAWA Common Alarm Code Definition
 *   110 Light sensor
 *      110 = The light sensor failed to initialize; please check the module integration and wiring.
 *      111 = The light sensor measurement is abnormal; please check the module integrity.
 *      112 = The light sensor measurement is showing an extreme value; please monitor the device's operation closey.
 *
 *   120 Weather sensor
 *      120 = The weather sensor failed to initialize; please check the module integration and wiring.
 *      121 = The weather sensor measurement is abnormal; The ambient temperature is out of range.
 *      122 = The weather sensor measurement is showing an extreme value; The ambient temperature is exceeding safe threshold; please monitor the device's operation closely.
 *      123 = The weather sensor measurement is showing an extreme value; The ambient temperature is less than safe threshold; please monitor the device's operation closely.
 *      124 = The weather sensor measurement is abnormal; The ambient humidity is out of range.
 *      125 = The weather sensor measurement is showing an extreme value; The ambient humidity is exceeding safe threshold; please monitor the device's operation closely.
 *      126 = The weather sensor measurement is showing an extreme value; The ambient humidity is below safe threshold; please monitor the device's operation closely.
 *      127 = The weather sensor measurement is abnormal; The barometric pressure is out of range.
 *      128 = The weather sensor measurement is showing an extreme value; The barometric pressure is more than safe threshold; please monitor the device's operation closely.
 *      129 = The weather sensor measurement is showing an extreme value; The barometric pressure is less than safe threshold; please monitor the device's operation closely.
 *
 *   130 SD Card
 *      130 = The SD Card failed to initialize; please check the module integration and wiring.
 *      131 = The SD Card failed to attatch; please check if the card is inserted properly.
 *      132 = The SD Card failed to create log file; please check if the card is ok.
 *      133 = The SD Card failed to write to the log file; please check if the card is ok.
 * 
 *   140 AC Power sensor
 *      140 = The power sensor failed to initialize; please check the module integration and wiring.
 *      141 = The power sensor measurement is abnormal; The voltage reading is out of range.
 *      142 = The power sensor measurement is abnormal; The current reading is out of range.
 *      143 = The power sensor measurement is abnormal; The power reading is out of range.
 *      144 = The power sensor measurement is abnormal; The power factor and frequency reading is out of range.
 *      145 = The power sensor measurement is showing an overlimit; Please check the connected instruments.
 * 
 *   150 Real Time Clock
 *      150 = The device timing information is incorrect; please update the device time manually. Any function that requires precise timing will malfunction!
 * 
 *   210 Switch Relay
 *      211 = Switch number one is active, but the power sensor detects no power utilization. Please check the connected instrument to prevent failures.
 *      212 = Switch number two is active, but the power sensor detects no power utilization. Please check the connected instrument to prevent failures.
 *      213 = Switch number three is active, but the power sensor detects no power utilization. Please check the connected instrument to prevent failures.
 *      214 = Switch number four is active, but the power sensor detects no power utilization. Please check the connected instrument to prevent failures.
 *      215 = All switches are inactive, but the power sensor detects large power utilization. Please check the device relay module to prevent relay malfunction.
 *      216 = Switch numner one is active for more than safe duration!
 *      217 = Switch number two is active for more than safe duration!
 *      218 = Switch number three is active for more than safe duration!
 *      219 = Switch number four is active for more than safe duration! 
 * 
 */
#endif
