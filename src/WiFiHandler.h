//
// Created by JanHe on 09.01.2026.
//

#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H


class WiFiHandler
{
private:
    static void checkConnection();

public:
    static void setup();
    static void loop();
};


#endif //WIFIHANDLER_H
