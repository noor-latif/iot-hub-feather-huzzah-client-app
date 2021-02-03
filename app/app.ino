/* Copyright (c) Microsoft. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Please use an Arduino IDE 1.6.8 or greater

   Don't forget to run the python script inside the sketch folder before uploading this sketch.
   It won't work otherwise! (Does NOT work with Microsoft Store version of Arduino)
*/
// Follow the 7 step configuration below before uploading the sketch.
// 0. Run the .py script inside the sketch folder! (Does NOT work with Microsoft Store version of Arduino)
// 1. Device ID name that you chose when creating your device.
#define DEVICE_ID "Feather HUZZAH ESP8266 WiFi"
// 2. If you don't have a physical DHT sensor, send simulated data to IoT hub.
#define SIMULATED_DATA false
// 3. Set the DHT pin. Set DHT22 or DHT11 is supported.
#define DHT_TYPE DHT11
#define DHT_PIN D1
// 4. Send an alert when we pass a certain temperature degree.
#define TEMPERATURE_ALERT 25
// 5. LED_PIN flashes on successful messaging. LED will blink repeatedly if there is an error.
#define LED_PIN D4
// 6. Interval time(ms) for sending message to IoT Hub.
#define INTERVAL 10000
// 7. Enter the DEVICE connection string found in your created device under "IoT Devices" in Azure.
//    NOT the iothubowner connection strings or the like.
static char *deviceConnectionString = "paste_string_here";
static char *ssid = "wifi_name";     // Wi-Fi name
static char *password = "password";  // Wi-Fi password
#define MESSAGE_MAX_LEN 256

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <Adafruit_Sensor.h>  // 'Adafruit Unified Sensor'. search for "sensor abstraction layer" in lib manager
#include <DHT.h>              //  DHT sensor library (by Adafruit)
static DHT dht(DHT_PIN, DHT_TYPE);  // Create dht object from DHT class

#include <AzureIoTHub.h>            // Install 'AzureIoTHub' by microsoft.
#include <AzureIoTProtocol_MQTT.h>  // Install 'AzureIoTProtocol MQTT' by microsoft.
#include <AzureIoTUtility.h>        // Install 'AzureIoTUtility' by microsoft.

#include "mqtt_protocol.h"  // for MQTT_protocol definitions
static bool messagePending = false; // Pending
static bool messageSending = true;  // Sending
static int interval = INTERVAL;     // Send message according to defined delay
static int messageCount = 1;

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
uint8_t mac[6]; // For storing the MAC Adress array
void setup()
{
  pinMode(LED_PIN, OUTPUT);     // Set LED as output (So we can send an on/off signal to it)
  digitalWrite(LED_PIN, LOW);   // Turn on the LED (ESP8266 HIGH/LOW logic is reversed)
  Serial.begin(115200);
  dht.begin();    // initialise the DHT sensor.
  connectWifi();  // Connects you to Wi-Fi
  clockSync();    // Connects to an NTP server to get epoch time

  iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(deviceConnectionString, MQTT_Protocol);
  if (iotHubClientHandle == NULL)
  {
    Serial.println("Connection string error. Use a DEVICE connection string! \nPreferably with up to date syntax from your HUB at portal.azure.com");
    while(1){
      blinkLED();
    }
  }

  IoTHubClient_LL_SetOption(iotHubClientHandle, "product_info", "HappyPath_AdafruitFeatherHuzzah-C");
  IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
  IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
  IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);
}

void loop()
{
  if (!messagePending && messageSending)
  {
    char messagePayload[MESSAGE_MAX_LEN];
    bool temperatureAlert = readMessage(messageCount, messagePayload);
    sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
    messageCount++;
    delay(interval);
  }
  IoTHubClient_LL_DoWork(iotHubClientHandle);
  delay(10);
}

void blinkLED()
{
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
}

void connectWifi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.macAddress(mac);
  Serial.printf("Device MAC address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(LED_PIN, HIGH);
  Serial.println("");
  Serial.print("Wi-Fi connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void clockSync()
{
  time_t epochTime;
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  while (true)
  {
    epochTime = time(NULL);

    if (epochTime == 0)
    {
      Serial.println("Failed to get time from NTP server! Re-trying in 2 seconds.");
      delay(2000);
    }
    else
    {
      Serial.printf("Epoch time is: %lu.\r\n", epochTime);
      break;
    }
  }
}
