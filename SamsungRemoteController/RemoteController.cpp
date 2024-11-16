#include "RemoteController.h"

RemoteController::RemoteController(bool powerOn, size_t temp, Modes mode, FanSpeeds fanSpeed, bool swing, uint8_t sendPin)
  : m_Values(powerOn, temp, mode, fanSpeed, swing), 
   m_Signal{ 0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00 },
   irsend(sendPin)
  {
    m_Signal[10] = 0x71;
    m_Signal[13] = 0xF0;
    SetSignal(m_Values);
    irsend.begin();
  };

RemoteController::RemoteController(const RemoteValues& values, uint8_t sendPin)
  : m_Values(values),
   m_Signal{ 0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00 }, 
   irsend(sendPin)
  {
    m_Signal[10] = 0x71;
    m_Signal[13] = 0xF0;

    SetSignal(values);
    irsend.begin();
  };

void RemoteController::SendSignal(){
  if(m_Values.PowerOn){
    irsend.sendSamsungAC(m_Signal.data());
    return;
  }
  irsend.sendSamsungAC(c_OffSignal.data(), 21);
}

const RemoteValues& RemoteController::GetValues() const {
  return m_Values;
}

void RemoteController::SetSignal(const RemoteValues& values){
  SetAutoSwing(values.AutoSwing);
  SetTemp(values.Temp);
  SetMode(values.Mode);
  SetFanSpeed(values.FanSpeed);
  SetChecksum();
}

void RemoteController::SetPower(bool powerOn){
  m_Values.PowerOn = powerOn;
  if(powerOn)
    SetChecksum();
}

void RemoteController::SetTemp(const uint8_t temp) {
  m_Values.Temp = temp;
  m_Signal[11] = (temp - 16) * 16;
  SetChecksum();
}

void RemoteController::SetAutoSwing(const bool autoSwing){
  m_Values.AutoSwing = autoSwing;
  m_Signal[9] = autoSwing ? 0xAE : 0xFE;
  SetChecksum();
}

void RemoteController::SetMode(const Modes mode) {
  m_Values.Mode = mode;
  m_Signal[12] = static_cast<uint8_t>(mode);
  m_Signal[12] -= IsModeDryOrAuto(mode)
      ? 0
      : static_cast<uint8_t>(m_Values.FanSpeed);
  SetChecksum();
}

void RemoteController::SetFanSpeed(const FanSpeeds fanSpeed) {
  m_Values.FanSpeed = fanSpeed;
  auto mode = m_Values.Mode;
  m_Signal[12] = (static_cast<uint8_t>(mode) - (IsModeDryOrAuto(mode)
      ? 0
      : static_cast<uint8_t>(fanSpeed)));
  SetChecksum();
}

const bool RemoteController::IsModeDryOrAuto(const Modes mode) const {
  return mode == Modes::Auto || mode == Modes::Dry;
}

void RemoteController::SetChecksum() {
  uint8_t checksum = 0x00;

  // Calculate the byte 8 checksum
  // Count the number of ONE bits
  m_Signal[9] &= 0b11111110;
  for (uint8_t j = 9; j < 13; j++) {
    uint8_t samsungByte = m_Signal[j];
    for (uint8_t i = 0; i < 8; i++) {
      if ((samsungByte & 0x01) == 0x01) {
        checksum++;
      }
      samsungByte >>= 1;
    }
  }

  checksum = 28 - checksum;
  checksum <<= 4;
  checksum |= 0x02;

  m_Signal[8] = checksum;
}