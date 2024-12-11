#pragma once

#include <LittleFS.h>
#include <functional>
#include <ArduinoJson.h>
#include "IntervalExecutor.h"
#include "RemoteValues.h"

class RemoteControllerFileManager {
private:
  IntervalExecutor<RemoteValues> m_Executer;
  const String m_Path;
public:
  RemoteControllerFileManager(String&& path);

public:
  void WriteRemoteValues(const RemoteValues& values);
  RemoteValues ReadValues();

private:
  void m_WriteJson(const RemoteValues& values);
  void m_WriteToFile(const String& content);
  const String m_SerializeRemoteValues(const RemoteValues& values);
  RemoteValues m_DeserializeRemoteValues(const String& json);
};