#pragma once

#include <array>
#include <IRremoteESP8266.h>
#include <IRsend.h>

enum class Modes : uint8_t {
    Auto = 0x0D,
    Cooling = 0x1F,
    Dry = 0x21,
    Fan = 0x3F,
    Heating = 0x4F
};

enum class FanSpeeds : uint8_t {
    VeryHigh = 0,
    High = 4,
    Medium = 6,
    Low = 10,
    Auto = 14,
};

struct RemoteValues {
public:
    bool PowerOn;
    size_t Temp;
    Modes Mode;
    FanSpeeds FanSpeed;
    bool AutoSwing;

    RemoteValues() = delete;

    RemoteValues(bool powerOn, size_t temp, Modes mode, FanSpeeds fanSpeed, bool swing)
        : PowerOn(powerOn), Temp(temp), Mode(mode), FanSpeed(fanSpeed), AutoSwing(swing) {}
};

class RemoteController {
private:
    RemoteValues m_Values;
    std::array<uint8_t, 14> m_Signal;
    const std::array<uint8_t, 21> c_OffSignal{ 0x02, 0xB2, 0x0F, 0x00, 0x00, 0x00, 0xC0, 0x01, 0xD2, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0xFF, 0x71, 0x80, 0x11, 0xC0 };
    IRsend irsend;
public:
    RemoteController() = delete;

    RemoteController(bool powerOn, size_t temp, Modes mode, FanSpeeds fanSpeed, bool swing, uint8_t sendPin);
    RemoteController(const RemoteValues& values, uint8_t sendPin);
public:
    void SendSignal();
    const RemoteValues& GetValues() const;
    void SetSignal(const RemoteValues& values);
    void SetPower(bool powerOn);
    void SetTemp(const uint8_t temp);
    void SetAutoSwing(const bool autoSwing);
    void SetMode(const Modes mode);
    void SetFanSpeed(const FanSpeeds fanSpeed);
private:
    const bool IsModeDryOrAuto(const Modes mode) const;
    void SetChecksum();
};

