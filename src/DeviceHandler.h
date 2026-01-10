//
// Created by JanHe on 10.01.2026.
//

#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H
#include <cstdint>


class DeviceHandler
{
private:
    static void handleBlink();

public:
    static void loop();
    static void setRelais(int8_t relais, bool state);
    static void setup();
};


#endif //DEVICEHANDLER_H
