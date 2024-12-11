#include "RemoteControllerFileManager.h"

RemoteControllerFileManager::RemoteControllerFileManager(String&& path)
    : m_Path(path),
      m_Executer(30000, [this](RemoteValues values){m_WriteJson(values);})
      {}

void RemoteControllerFileManager::WriteRemoteValues(const RemoteValues& values) {
  m_Executer.Execute(values);
}

RemoteValues RemoteControllerFileManager::ReadValues() {
  File file = LittleFS.open(m_Path, "r");
  if (!file)
  {
    file.close();
    return RemoteValues(false, 22, Modes::Cooling, FanSpeeds::High, false);
  }

  String json = file.readString();
  file.close();

  return m_DeserializeRemoteValues(json);
}

void RemoteControllerFileManager::m_WriteJson(const RemoteValues& values) {
  const String json = m_SerializeRemoteValues(values);
  m_WriteToFile(json);
}

void RemoteControllerFileManager::m_WriteToFile(const String& content) {
  File file = LittleFS.open("/remote.json", "w+");
  file.print(content);
  file.close();
}

const String RemoteControllerFileManager::m_SerializeRemoteValues(const RemoteValues& values) {
  JsonDocument jsonDoc;
  jsonDoc["PowerOn"] = values.PowerOn;
  jsonDoc["Temp"] = values.Temp;
  jsonDoc["Mode"] = (int)values.Mode;
  jsonDoc["FanSpeed"] = (int)values.FanSpeed;
  jsonDoc["AutoSwing"] = values.AutoSwing;

  String json;
  serializeJson(jsonDoc, json);
  return json;
}

RemoteValues RemoteControllerFileManager::m_DeserializeRemoteValues(const String& json) {
  JsonDocument doc;
  deserializeJson(doc, json);

  bool powerOn = doc["PowerOn"];
  uint8_t temp = doc["Temp"];
  Modes mode = (Modes)(uint8_t)(doc["Mode"]);
  FanSpeeds fanSpeed = (FanSpeeds)(uint8_t)doc["FanSpeed"];
  bool autoSwing = doc["AutoSwing"];

  return RemoteValues(powerOn, temp, mode, fanSpeed, autoSwing);
}
