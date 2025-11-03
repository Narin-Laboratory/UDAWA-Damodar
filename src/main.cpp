#include <ArduinoJson.h>
#include "main.h"
#include "set.h"

void setup(){
    loggingSetup();
    storageSetup();
    networkingSetup();
    coreroutineSetup();

    setCredentials();
}

void loop(){
    coreroutineLoop();
}