// Physical device information for board and sensor
#define DEVICE_ID "Feather HUZZAH ESP8266 WiFi"
#define DHT_TYPE DHT11

// Pin layout configuration
#define LED_PIN 0
#define DHT_PIN 2

#define TEMPERATURE_ALERT 30

// Interval time(ms) for sending message to IoT Hub
#define INTERVAL 10000

// If don't have a physical DHT sensor, can send simulated data to IoT hub
#define SIMULATED_DATA true

// SSID and SSID password's length should < 32 bytes
// http://serverfault.com/a/45509
#define SSID_LEN 32
#define PASS_LEN 32
#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256

#ifndef IOTHUBTRANSPORTMQTT_H
#define IOTHUBTRANSPORTMQTT_H

/* 
 * MQTT Protocol setup
 */
#include "iothub_transport_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif
    extern const TRANSPORT_PROVIDER* MQTT_Protocol(void);

#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORTMQTT_H*/
