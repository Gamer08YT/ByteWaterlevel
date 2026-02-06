//
// Created by JanHe on 10.01.2026.
//
#pragma once
#include <Arduino.h>


#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H


class DeviceHandler
{
private:
    static void handleRelais();
    static void handleBlink();
    static void scanSensors();
    static void handleScan();

public:
    static void loop();
    static void setRelais(int8_t relais, bool state);
    static void setup();
    static void setRelaisDuration(int as, int as1);
    static bool getState(int i);
    static float getADCValue();
    static float getCurrent(bool newReading);
    static float readVoltage(int pin, int samples);
    static float getCPUTemperature();
    static int getDuration(int i);
    static float getLevel();
    static float getVolume();
    static float getVolumeCached();
    static float getCPUTemperatureCached();
    static float getLevelCached();
    static float getCurrentCached();
    static float getADCValueCached();
};


#endif //DEVICEHANDLER_H
