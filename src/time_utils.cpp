#include <Arduino.h>
#include "time_utils.h"
#include "time.h"

// NTP server and timezone settings
static const char *ntpServer = "pool.ntp.org";
static const long gmtOffset_sec = 3600;     // Adjust for your timezone (seconds)
static const int daylightOffset_sec = 3600; // Daylight savings
struct tm timeinfo;

void InitTime()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Time initialized");
}

bool GetLocalTimeSafe(struct tm &timeinfo)
{
    // Wrapper around getLocalTime that returns a bool
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time from NTP server");
        return false;
    }
    return true;
}