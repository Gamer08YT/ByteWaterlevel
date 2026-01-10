//
// Created by JanHe on 10.01.2026.
//

#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H
#include <cstdint>

#include "ArduinoJson/Object/MemberProxy.hpp"
#include "ArduinoJson/Variant/JsonVariant.hpp"


class DeviceHandler
{
private:
    static void handleRelais();
    static void handleBlink();

public:
    static void loop();
    static void setRelais(int8_t relais, bool state);
    static void setup();
    static void setRelaisDuration(int as, int as1);
    static bool getState(int i);
    static uint16_t getADCValue();
};


#endif //DEVICEHANDLER_H
