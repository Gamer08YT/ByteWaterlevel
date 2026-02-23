//
// Created by JanHe on 10.01.2026.
//

#ifndef INTERNALCONFIG_H
#define INTERNALCONFIG_H

/**
 * Define Default Values.
 */
#define VERSION "1.5.0"
#define DEBUG true
#define BLINK_INTERVAL 250
#define SCAN_INTERVAL 1000
#define MQTT_INTERVAL 1000
#define AUTO_INTERVAL 1000

/**
 * Define EMA Filtering
 * 0.05 very good, very slow => 5% from new Value
 * 0.30 ok, ok => 30% from new Value
 * 0.50 very bad, very fast => 50% from new Value
 */
#define EMA_ALPHA 0.3

/**
 * Define Pinouts.
 */
#define LED_PIN 2
#define RELAIS_CH1 0
#define RELAIS_CH2 1
#define SENSE 3
#define RESET 6

/**
 * Define default Automation.
 */

#define PUMP_FILL 1
#define PUMP_EMPTY 2

/**
 * Define Update Server
 */
#define UPDATE_SERVER "https://space.byte-store.de/external/waterlevel/versions.json"

#endif //INTERNALCONFIG_H
