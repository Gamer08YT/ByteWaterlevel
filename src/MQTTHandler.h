//
// Created by JanHe on 10.01.2026.
//

#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H


class MQTTHandler
{
public:
    static void setup();
    static void publish(const char* topic, const char* payload);
    static void loop();
};


#endif //MQTTHANDLER_H
