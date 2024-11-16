#include <ArduinoJson.h>
#include <LittleFS.h>
#include "RemoteWebHost.h"
#include "RemoteController.h"
#include "Secrets.h"

#define FORMAT_LITTLEFS_IF_FAILED false

bool InitLittleFS();
void MapEndpoints();
String SerializeRemoteValues(RemoteValues& values);
void UpdatePower(AsyncWebServerRequest* request, uint8_t* data);
void UpdateSwing(AsyncWebServerRequest* request, uint8_t* data);
void UpdateMode(AsyncWebServerRequest* request, uint8_t* data);
void UpdateFanSpeed(AsyncWebServerRequest* request, uint8_t* data);
void UpdateTemp(AsyncWebServerRequest* request, uint8_t* data);

constexpr int LED_PIN = 5;
RemoteController remoteController(false, 22, Modes::Cooling, FanSpeeds::High, false, LED_PIN);
RemoteWebHost webHost(WIFI_SSID, WIFI_PASSWORD); //#define your WIFI_SSID and WIFI_PASSWORD in Secrets.h


void setup() {
  Serial.begin(115200);
  Serial.println("\n================================");
  
  if(!InitLittleFS()) 
    return;

  MapEndpoints();
  webHost.Init();
}

void loop() {
  yield();
}

bool InitLittleFS(){
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LITTLEFS Mount Failed.");
    return false;
  }

  Serial.println("LITTLEFS Mount SUCCESSFUL.");
  Serial.println("\n================================");

  return true;
}

void MapEndpoints(){
  webHost.MapPost("/temp", UpdateTemp)
    .MapPost("/fan", UpdateFanSpeed)
    .MapPost("/toggle-swing", UpdateSwing)
    .MapPost("/power", UpdatePower)
    .MapPost("/mode", UpdateMode);

  typedef AsyncWebServerRequest* Request;
    
  webHost.MapGet("/", [](Request request) {
      request->send(LittleFS, "/index.html", "text/html");
    })
    .MapGet("/styles.css", [](Request request) {
      request->send(LittleFS, "/styles.css", "text/css");
    })
    .MapGet("/script.js", [](Request request) {
      request->send(LittleFS, "/script.js", "application/javascript");
    })
    .MapGet("/data", [](Request request) {
      request->send(200, "application/json", SerializeRemoteValues(remoteController.GetValues()));
    });
}





void UpdateTemp(AsyncWebServerRequest* request, uint8_t* data){
  StaticJsonDocument<256> doc;
  deserializeJson(doc, data);
  const uint8_t temp = doc["temp"].as<uint8_t>();
  Serial.print("Sent temp: "); Serial.println(temp);
  if(temp == 0){
    return;
  }
  remoteController.SetTemp(temp);
  remoteController.SendSignal();
}

void UpdateFanSpeed(AsyncWebServerRequest* request, uint8_t* data){
  StaticJsonDocument<256> doc;
  deserializeJson(doc, data);
  uint8_t fanSpeed = doc["fan"].as<uint8_t>();
  remoteController.SetFanSpeed(GetFanSpeedByIndex(fanSpeed));
  remoteController.SendSignal();
  Serial.print("Sent fan speed: "); Serial.println(fanSpeed);
}

void UpdateMode(AsyncWebServerRequest* request, uint8_t* data){
  StaticJsonDocument<256> doc;
  deserializeJson(doc, data);
  uint8_t mode = doc["mode"].as<uint8_t>();
  remoteController.SetMode(GetModeByIndex(mode));
  remoteController.SendSignal();
  Serial.print("Sent mode: "); Serial.println(mode);
}

void UpdateSwing(AsyncWebServerRequest* request, uint8_t* data){
  bool swing = !remoteController.GetValues().AutoSwing;
  remoteController.SetAutoSwing(swing);
  remoteController.SendSignal();
  Serial.print("Sent swing: "); Serial.println(swing);
}

void UpdatePower(AsyncWebServerRequest* request, uint8_t* data){
  StaticJsonDocument<256> doc;
  deserializeJson(doc, data);
  bool powerOn = doc["powerOn"].as<bool>();
  remoteController.SetPower(powerOn);
  remoteController.SendSignal();
  Serial.print("Sent power: "); Serial.println(powerOn);
}

FanSpeeds GetFanSpeedByIndex(int fanSpeed){
  switch(fanSpeed){
    case 0: return FanSpeeds::Auto;
    case 1: return FanSpeeds::Low;
    case 2: return FanSpeeds::Medium;
    case 3: return FanSpeeds::High;
    case 4: return FanSpeeds::VeryHigh;
    default: return FanSpeeds::Low;
  }
}

Modes GetModeByIndex(int mode){
  switch (mode) {
    case 0: return Modes::Auto;
    case 1: return Modes::Cooling;
    case 2: return Modes::Dry;
    case 3: return Modes::Fan;
    case 4: return Modes::Heating;
    default: return Modes::Fan;
  }
}

const String SerializeRemoteValues(const RemoteValues& values) {
  StaticJsonDocument<200> jsonDoc;
  int mode = values.Mode == Modes::Auto ? 0
    : values.Mode == Modes::Cooling ? 1
    : values.Mode == Modes::Dry ? 2
    : values.Mode == Modes::Fan ? 3
    : 4; // heating;
  int fanSpeed = values.FanSpeed == FanSpeeds::VeryHigh ? 4
    : values.FanSpeed == FanSpeeds::High ? 3
    : values.FanSpeed == FanSpeeds::Medium ? 2
    : values.FanSpeed == FanSpeeds::Low ? 1
    : 0; // Auto;

  jsonDoc["PowerOn"] = values.PowerOn;
  jsonDoc["Temp"] = values.Temp;
  jsonDoc["Mode"] = mode;
  jsonDoc["Fan"] = fanSpeed;
  jsonDoc["AutoSwing"] = values.AutoSwing;
  
  String json;
  serializeJson(jsonDoc, json);
  return json;
}
