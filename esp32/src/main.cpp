/**
 * @file main.cpp
 * @author Narges (n.kurkani@gmail.com)
 * @brief This code is written to get date and time from ntp server
 * using WiFi connection and send it to FRDM-K64F board using UART and set the RTC module of FRDM-K64F board.
 * @version 0.1
 * @date 2025-04-15
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

const char *SSID = "NETGEAR08";          // Change it to your WiFi SSID
const char *PASSWORD = "mistypotato283"; // Change it to your WiFi password

const char *ntpserver = "pool.ntp.org";
const long gmtOffset_sec = 3600; // Adjust based on your timezone (e.g., +1 hour)
const int daylightOffset_sec = 3600;

#define UART Serial1
void setup()
{
    Serial.begin(115200);
    UART.begin(115200);
    WiFi.begin(SSID, PASSWORD);
    Serial.println("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    configTime(gmtOffset_sec, daylightOffset_sec, ntpserver); // CET
    Serial.println("Synchronizing time .");
    struct tm timeinfo = {0};
    while (!getLocalTime(&timeinfo))
    {
        Serial.print(".");
    }

    WiFi.disconnect();
}

void loop()
{
    char datetime[20]{0};

    time_t now{time(nullptr)};
    struct tm *timeinfo = localtime(&now);
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", timeinfo);

    Serial.println(datetime);
    UART.println(datetime);
    delay(1000);
}