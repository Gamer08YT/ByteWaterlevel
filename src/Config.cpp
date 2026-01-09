//
// Created by JanHe on 09.01.2026.
//
#include <WString.h>

struct Config
{
    String name;

    struct
    {
        struct
        {
            String ssid;
            String password;
        } client;

        struct
        {
            String password;
        } ap;
    } wifi;

    struct
    {
        bool led;
    } hardware;
};

Config config;
