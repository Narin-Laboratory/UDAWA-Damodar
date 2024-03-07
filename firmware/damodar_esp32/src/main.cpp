/**
 * UDAWA - Universal Digital Agriculture Watering Assistant
 * Firmware for Vanilla UDAWA Board (Starter Kit)
 * Licensed under aGPLv3
 * Researched and developed by PRITA Research Group & Narin Laboratory
 * prita.undiknas.ac.id | narin.co.id
**/
#include "main.h"

void setup()
{
  processSharedAttributeUpdateCb = &attUpdateCb;
  onTbDisconnectedCb = &onTbDisconnected;
  onTbConnectedCb = &onTbConnected;
  processSetPanicCb = &setPanic;
  processGenericClientRPCCb = &genericClientRPC;
  emitAlarmCb = &onAlarm;
  onSyncClientAttrCb = &onSyncClientAttr;
  onSaveSettings = &saveSettings;
  #ifdef USE_WEB_IFACE
  wsEventCb = &onWsEvent;
  #endif
  onMQTTUpdateStartCb = &onMQTTUpdateStart;
  onMQTTUpdateEndCb = &onMQTTUpdateEnd;
  
  
  startup();
  if(!config.SM){
    loadSettings();
    syncConfigCoMCU();
  }
  if(String(config.model) == String("Generic")){
    strlcpy(config.model, "Damodar", sizeof(config.model));
  }

  tb.setBufferSize(1024);

  #ifdef USE_WEB_IFACE
  xQueueWsPayloadSensors= xQueueCreate( 1, sizeof( struct WSPayloadSensors ) );
  #endif


  if(xHandlePublishDevTel == NULL && !config.SM){
    xReturnedPublishDevTel = xTaskCreatePinnedToCore(publishDeviceTelemetryTR, PSTR("publishDevTel"), STACKSIZE_PUBLISHDEVTEL, NULL, 1, &xHandlePublishDevTel, 1);
    if(xReturnedPublishDevTel == pdPASS){
      log_manager->warn(PSTR(__func__), PSTR("Task publishDevTel has been created.\n"));
    }
  }

  #ifdef USE_WEB_IFACE
  if(xHandleWsSendTelemetry == NULL && !config.SM){
    xReturnedWsSendTelemetry = xTaskCreatePinnedToCore(wsSendTelemetryTR, PSTR("wsSendTelemetry"), STACKSIZE_WSSENDTELEMETRY, NULL, 1, &xHandleWsSendTelemetry, 1);
    if(xReturnedWsSendTelemetry == pdPASS){
      log_manager->warn(PSTR(__func__), PSTR("Task wsSendTelemetry has been created.\n"));
    }
  }
  #endif

  if(xHandleSensors == NULL && !config.SM){
    xReturnedSensors = xTaskCreatePinnedToCore(sensorsTR, PSTR("sensors"), STACKSIZE_SENSORS, NULL, 1, &xHandleSensors, 1);
    if(xReturnedPublishDevTel == pdPASS){
      log_manager->warn(PSTR(__func__), PSTR("Task sensors has been created.\n"));
    }
  }

}

void loop(){
  udawa();
  if(!config.SM){
    
  }
  vTaskDelay((const TickType_t) 1000 / portTICK_PERIOD_MS);
}

void setPanic(const RPC_Data &data){
  
}

void loadSettings()
{
  StaticJsonDocument<DOCSIZE_SETTINGS> doc;
  readSettings(doc, settingsPath);
  if(config.logLev == 5){
    log_manager->debug(PSTR(__func__), PSTR(": "));
    serializeJson(doc, Serial);
    Serial.println("\n");
  }

  if(doc["itD"] != nullptr){mySettings.itD = doc["itD"].as<uint16_t>();}
  else{mySettings.itD = 15;}

  if(doc["s1rx"] != nullptr){mySettings.s1rx = doc["s1rx"].as<uint8_t>();}
  else{mySettings.s1rx = 32;}

  if(doc["s1tx"] != nullptr){mySettings.s1tx = doc["s1tx"].as<uint8_t>();}
  else{mySettings.s1tx = 33;}
}

void saveSettings()
{
  StaticJsonDocument<DOCSIZE_SETTINGS> doc;

  doc["itD"] = mySettings.itD;

  doc["s1tx"] = mySettings.s1tx;
  doc["s1rx"] = mySettings.s1rx;

  writeSettings(doc, settingsPath);

  if(config.logLev == 5){
    log_manager->debug(PSTR(__func__), PSTR(": "));
    serializeJson(doc, Serial);
    Serial.println("\n");
  }
  log_manager->verbose(PSTR(__func__), PSTR("Settings saved.\n"));
}


void attUpdateCb(const Shared_Attribute_Data &data)
{
  if( xSemaphoreSettings != NULL ){
    if( xSemaphoreTake( xSemaphoreSettings, ( TickType_t ) 1000 ) == pdTRUE )
    {
      if(data["itD"] != nullptr){mySettings.itD = data["itD"].as<uint16_t>();}
      
      if(data["s1tx"] != nullptr){mySettings.s1tx = data["s1tx"].as<uint8_t>();}
      if(data["s1rx"] != nullptr){mySettings.s1rx = data["s1rx"].as<uint8_t>();}

      xSemaphoreGive( xSemaphoreSettings );
    }
    else
    {
      log_manager->verbose(PSTR(__func__), PSTR("No semaphore available.\n"));
    }
  }
}

void onTbConnected(){
  
}

void onTbDisconnected(){
 
}



RPC_Response genericClientRPC(const RPC_Data &data){
  if(data[PSTR("cmd")] != nullptr){
      const char * cmd = data["cmd"].as<const char *>();
      log_manager->verbose(PSTR(__func__), PSTR("Received command: %s\n"), cmd);

      if(strcmp(cmd, PSTR("commandExample")) == 0){
        
      }
      else if(strcmp(cmd, PSTR("dlCfg")) == 0){
        if(data[PSTR("dlCfg")] != nullptr){
          if(data[PSTR("dlCfg")].as<uint8_t>() == 1){
            processSharedAttributeUpdate(data);

            if(data[PSTR("sav")] != nullptr){
              if(data[PSTR("sav")].as<uint8_t>() == 1){
                FLAG_SAVE_CONFIGCOMCU = 1;
                FLAG_SYNC_CONFIGCOMCU = 1;
              }
              else if(data[PSTR("sav")].as<uint8_t>() == 2){
                FLAG_SAVE_CONFIG = 1;
              }
              else if(data[PSTR("sav")].as<uint8_t>() == 3){
                FLAG_SAVE_SETTINGS = 1;
              }
            }
          }
        }
      }
  }  
    
  return RPC_Response(PSTR("generic"), 1);
}

void deviceTelemetry(){
    if(config.provSent && tb.connected() && config.fIoT){
      StaticJsonDocument<128> doc;
      char buffer[128];
      
      doc[PSTR("uptime")] = millis(); 
      doc[PSTR("heap")] = heap_caps_get_free_size(MALLOC_CAP_8BIT); 
      doc[PSTR("rssi")] = WiFi.RSSI(); 
      doc[PSTR("dt")] = rtc.getEpoch(); 

      serializeJson(doc, buffer);
      tbSendAttribute(buffer);
    }
}

void onAlarm(int code){
  char buffer[32];
  StaticJsonDocument<32> doc;
  doc[PSTR("alarm")] = code;
  serializeJson(doc, buffer);
  #ifdef USE_WEB_IFACE
  wsBroadcastTXT(buffer);
  #endif
}

void publishDeviceTelemetryTR(void * arg){
  unsigned long timerDeviceTelemetry = millis();
  while(true){

    
    unsigned long now = millis();
    if( (now - timerDeviceTelemetry) > mySettings.itD * 1000 ){
      deviceTelemetry();
      timerDeviceTelemetry = now;
    }
    
    vTaskDelay((const TickType_t) 300 / portTICK_PERIOD_MS);
  }
}

void onSyncClientAttr(uint8_t direction){
    long startMillis = millis();

    StaticJsonDocument<1024> doc;
    char buffer[1024];
    

    if(tb.connected() && (direction == 0 || direction == 1)){
      doc[PSTR("itD")] = mySettings.itD;
      doc[PSTR("s1rx")] = mySettings.s1rx;
      doc[PSTR("s1tx")] = mySettings.s1tx;
      serializeJson(doc, buffer);
      tbSendAttribute(buffer);
      doc.clear();
    }

    #ifdef USE_WEB_IFACE
    if(config.wsCount > 0 && (direction == 0 || direction == 2)){
      //doc[] = x;
      serializeJson(doc, buffer);
      wsBroadcastTXT(buffer);
    }
    #endif
    
    log_manager->verbose(PSTR(__func__), PSTR("Executed (%dms).\n"), millis() - startMillis);
}

#ifdef USE_WEB_IFACE
void onWsEvent(const JsonObject &doc){
  if(doc["evType"] == nullptr){
    log_manager->debug(PSTR(__func__), PSTR("Event type not found.\n"));
    return;
  }
  int evType = doc["evType"].as<int>();


  #ifdef USE_ASYNC_WEB
  if(evType == (int)WS_EVT_CONNECT){
  #endif
  #ifndef USE_ASYNC_WEB
  if(evType == (int)WStype_CONNECTED){
  #endif
    FLAG_SYNC_CLIENT_ATTR_2 = true;
  }
  #ifdef USE_ASYNC_WEB
  if(evType == (int)WS_EVT_DISCONNECT){
  #endif
  #ifndef USE_ASYNC_WEB
  else if(evType == (int)WStype_DISCONNECTED){
  #endif
      if(config.wsCount < 1){
          log_manager->debug(PSTR(__func__),PSTR("No WS client is active. \n"));
      }
  }
  #ifdef USE_ASYNC_WEB
  if(evType == (int)WS_EVT_DATA){
  #endif
  #ifndef USE_ASYNC_WEB
  else if(evType == (int)WStype_TEXT){
  #endif
    if(doc["cmd"] == nullptr){
        log_manager->debug(PSTR(__func__), "Command not found.\n");
        return;
    }
    const char* cmd = doc["cmd"].as<const char*>();
    if(strcmp(cmd, (const char*) "attr") == 0){
      processSharedAttributeUpdate(doc);
      FLAG_SYNC_CLIENT_ATTR_1 = true;
    }
    else if(strcmp(cmd, (const char*) "saveSettings") == 0){
      FLAG_SAVE_SETTINGS = true;
    }
    else if(strcmp(cmd, (const char*) "configSave") == 0){
      FLAG_SAVE_CONFIG = true;
    }
    else if(strcmp(cmd, (const char*) "saveState") == 0){
      FLAG_SAVE_STATES = true;
      log_manager->debug(PSTR(__func__), PSTR("FLAG_SAVE_STATES set to TRUE\n"));
    }
    else if(strcmp(cmd, (const char*) "setPanic") == 0){
      doc[PSTR("st")] = configcomcu.fP ? "OFF" : "ON";
      processSetPanic(doc);
    }
    else if(strcmp(cmd, (const char*) "reboot") == 0){
      reboot();
    }
    #ifdef USE_DISK_LOG
    else if(strcmp(cmd, (const char*) PSTR("wsStreamCardLogger")) == 0){
      GLOBAL_TARGET_CLIENT_ID = doc[PSTR("num")].as<uint32_t>(); 
      GLOBAL_LOG_FILE_NAME = doc[PSTR("fileName")].as<String>();
      FLAG_WS_STREAM_SDCARD = true;
    }
    #endif
  }
}


void wsSendTelemetryTR(void *arg){
  while(true){
    if(config.fIface && config.wsCount > 0){
      char buffer[128];
      StaticJsonDocument<128> doc;
      JsonObject devTel = doc.createNestedObject("devTel");
      devTel[PSTR("heap")] = heap_caps_get_free_size(MALLOC_CAP_8BIT);
      devTel[PSTR("rssi")] = WiFi.RSSI();
      devTel[PSTR("uptime")] = millis()/1000;
      devTel[PSTR("dt")] = rtc.getEpoch();
      devTel[PSTR("dts")] = rtc.getDateTime();
      serializeJson(doc, buffer);
      wsBroadcastTXT(buffer);
      doc.clear();
  
      if( xQueueWsPayloadSensors != NULL ){
        WSPayloadSensors payload;
        if( xQueueReceive( xQueueWsPayloadSensors,  &( payload ), ( TickType_t ) 0 ) == pdPASS )
        {
          if(payload.ppm != 0 && payload.cels != 0){
            JsonObject sensors = doc.createNestedObject("sensors");
            sensors[PSTR("ppm")] = payload.ppm;
            sensors[PSTR("cels")] = payload.cels;
            serializeJson(doc, buffer);
            wsBroadcastTXT(buffer);
            doc.clear();
          }
          
        }
      }

      #ifdef USE_DISK_LOG
      if(FLAG_WS_STREAM_SDCARD)
      {
        wsStreamCardLogger(GLOBAL_TARGET_CLIENT_ID, GLOBAL_LOG_FILE_NAME);
        FLAG_WS_STREAM_SDCARD = false;
        GLOBAL_TARGET_CLIENT_ID = 0;
        GLOBAL_LOG_FILE_NAME = "";
      }
      #endif
    }
    vTaskDelay((const TickType_t) 1000 / portTICK_PERIOD_MS);
  }
}
#endif

void onMQTTUpdateStart(){
  #ifdef USE_WEB_IFACE
  if(xHandleWsSendTelemetry != NULL){vTaskSuspend(xHandleWsSendTelemetry);}
  if(xHandleIface != NULL){vTaskSuspend(xHandleIface);}
  #endif
  if(xHandlePublishDevTel != NULL){vTaskSuspend(xHandlePublishDevTel);}
}

void onMQTTUpdateEnd(){
  
}

void sensorsTR(void *arg){
  //Stream_Stats<float> _cels_;
  //Stream_Stats<float> _ppm_;

  unsigned long timerAttribute = millis();
  unsigned long timerTelemetry = millis();

  while (true)
  {
    bool flag_failure_readings = false;
    myStates.flag_sensors = true;

    float cels = 0;
    float ppm = 0;

    //read TDS from nano
    unsigned long now = millis();
    if(myStates.flag_sensors && !flag_failure_readings)
    {
      StaticJsonDocument<DOCSIZE_MIN> doc;
      char buffer[DOCSIZE_MIN];
      
      if( (now - timerAttribute) > (mySettings.itSc * 1000))
      {
        doc[PSTR("method")] = PSTR("readSensors");
        serialWriteToCoMcu(doc, true, 50);
        if(doc[PSTR("cels")] != nullptr){cels = doc[PSTR("cels")].as<float>();}
        if(doc[PSTR("ppm")] != nullptr){ppm = doc[PSTR("ppm")].as<float>();}

        doc.clear();

        // _cels_.Add(cels);
        // _ppm_.Add(ppm);
        
        if(tb.connected() && config.provSent){
          doc[PSTR("_ppm")] = ppm;
          doc[PSTR("_cels")] = cels;
          serializeJson(doc, buffer);
          tbSendAttribute(buffer);
          doc.clear();
        }
        

        timerAttribute = now;
      }

      if( (now - timerTelemetry) > (mySettings.itS * 1000) )
      {
        if(ppm > 0 && ppm < 2000 && cels > 0 && cels < 100){
          doc[PSTR("cels")] = cels;//_cels_.Get_Average();  
          doc[PSTR("ppm")] = ppm;//_ppm_.Get_Average(); 

          #ifdef USE_DISK_LOG  
          writeCardLogger(doc);
          #endif
          if(tb.connected() && config.provSent)
          {
            serializeJson(doc, buffer);
            if(tbSendTelemetry(buffer)){
              
            }
          }
          doc.clear();
        }
        //_cels_.Clear(); _ppm_.Clear();
        timerTelemetry = now;
      }

    }

    #ifdef USE_WEB_IFACE
      if( xQueueWsPayloadSensors != NULL && (config.wsCount > 0) && myStates.flag_sensors && !flag_failure_readings)
      {
        WSPayloadSensors payload;
        payload.ppm = ppm;
        payload.cels = cels;
        if( xQueueSend( xQueueWsPayloadSensors, &payload, ( TickType_t ) 1000 ) != pdPASS )
        {
          log_manager->debug(PSTR(__func__), PSTR("Failed to fill WSPayloadSensors. Queue is full. \n"));
        }
      }
    #endif
    
    vTaskDelay((const TickType_t) 1000 / portTICK_PERIOD_MS);
  }
}
