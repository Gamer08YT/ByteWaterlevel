//
// Created by JanHe on 10.01.2026.
//

#ifndef INTERNALCONFIG_H
#define INTERNALCONFIG_H

/**
 * Define Default Values.
 */
#define VERSION "1.2.0"
#define DEBUG true
#define BLINK_INTERVAL 250
#define MQTT_INTERVAL 1000

/**
 * Define Pinouts.
 */
#define LED_PIN 2
#define RELAIS_CH1 0
#define RELAIS_CH2 1
#define SENSE 3
#define RESET 6

/**
 * Define Update Server
 */
#define UPDATE_SERVER "https://space.byte-store.de/external/waterlevel/versions.json"

#endif //INTERNALCONFIG_H
