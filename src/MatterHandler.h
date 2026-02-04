//
// Created by JanHe on 11.01.2026.
//

#ifndef MATTERHANDLER_H
#define MATTERHANDLER_H
#include <Matter.h>


class MatterHandler
{
private:
    static void beginPlugins();

public:
    static void setup();
    static void loop();
    static String getPairingCode();
};


#endif //MATTERHANDLER_H
