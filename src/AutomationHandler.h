//
// Created by JanHe on 09.02.2026.
//

#ifndef AUTOMATIONHANDLER_H
#define AUTOMATIONHANDLER_H


class AutomationHandler
{
private:
    static void handlePump();
    static void handleFill();

public:
    static void loop();
    static void setup();
};


#endif //AUTOMATIONHANDLER_H
