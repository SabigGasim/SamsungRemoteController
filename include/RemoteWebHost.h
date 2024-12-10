#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <map>

class RemoteWebHost {
public:
  typedef std::function<void(AsyncWebServerRequest* request, uint8_t* data)> PostFunction;
  typedef ArRequestHandlerFunction GetFunction;
  typedef std::map<String, PostFunction> PostMap;
  typedef std::map<String, GetFunction> GetMap;
private:
  PostMap m_PostMap;
  GetMap m_GetMap;
  const char* m_Network;
  const char* m_Password;
public:
  RemoteWebHost(const char* ssid, const char* password)
  :   m_Network(ssid),
      m_Password(password),
      server(80),                       
      local_IP(192, 168, 1, 169),       
      gateway(192, 168, 1, 1),          
      subnet(255, 255, 255, 0),         
      primaryDNS(8, 8, 8, 8),           
      secondaryDNS(8, 8, 4, 4) {}
public:
  RemoteWebHost& Init();
  RemoteWebHost& MapPost(const String& endpoint, PostFunction function);
  RemoteWebHost& MapGet(const String& endpoint, GetFunction function);
private:
  AsyncWebServer server;
  IPAddress local_IP;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress primaryDNS;
  IPAddress secondaryDNS;

  void ConfigureWifi();
  void InitWebHost();
};
