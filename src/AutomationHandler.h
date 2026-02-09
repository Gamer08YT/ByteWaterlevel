//
// Created by JanHe on 09.02.2026.
//

#ifndef AUTOMATIONHANDLER_H
#define AUTOMATIONHANDLER_H


class AutomationHandler
{
private:
    static void setPump(bool cond);
    static void handlePump();
    static void setFill(bool cond);
    static void handleFill();

public:
    static void loop();
    static void setup();
    static bool isFilling();
    static bool isPumping();
    static int getMode();
};


#endif //AUTOMATIONHANDLER_H
