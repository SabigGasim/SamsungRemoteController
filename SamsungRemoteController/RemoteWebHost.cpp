#include "RemoteWebHost.h"

RemoteWebHost& RemoteWebHost::Init(){
  this->InitWebHost();
  return *this;
}

RemoteWebHost& RemoteWebHost::MapPost(const String& endpoint, PostFunction function){
  this->m_PostMap[endpoint] = function;
  return *this;
}

RemoteWebHost& RemoteWebHost::MapGet(const String& endpoint, GetFunction function){
  this->m_GetMap[endpoint] = function;
  return *this;
}

void RemoteWebHost::ConfigureWifi(){
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
    return;
  }

  WiFi.begin(m_Network, m_Password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA);

  Serial.println("Connected to WiFi");
}

void RemoteWebHost::InitWebHost(){
  ConfigureWifi();

  for(const auto& endpoint : this->m_GetMap){
    server.on(endpoint.first.c_str(), HTTP_GET, endpoint.second);
  }

  server.onRequestBody([this](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
    const String& url = request->url();
    this->m_PostMap[url](request, data);
  });

  server.begin();

  Serial.println("Listening on: " + WiFi.localIP().toString());
}

