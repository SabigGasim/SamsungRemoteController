#pragma once
#include <cstdint>

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
    uint8_t Temp;
    Modes Mode;
    FanSpeeds FanSpeed;
    bool AutoSwing;

    RemoteValues(){}

    RemoteValues(bool powerOn, uint8_t temp, Modes mode, FanSpeeds fanSpeed, bool swing)
        : PowerOn(powerOn), Temp(temp), Mode(mode), FanSpeed(fanSpeed), AutoSwing(swing) {}
};