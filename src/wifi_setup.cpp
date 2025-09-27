#include <WiFi.h>
#include "wifi_setup.h"

// Wi-Fi credentials (edit as needed)
static const char *ssids[] = {"novabox", "Galaxy S25 Ultra"};
static const char *passwords[] = {"samilike", "87654321"};
static const int wifiCount = sizeof(ssids) / sizeof(ssids[0]);

void Connect_To_Wifi() {
  bool connected = false;
  for (int i = 0; i < wifiCount; i++) {
    Serial.printf("Connecting to %s\n", ssids[i]);
    WiFi.begin(ssids[i], passwords[i]);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // wait max ~10s
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      connected = true;
      break;
    } else {
      Serial.println("\nFailed, trying next network...");
    }
  }

  if (!connected) {
    Serial.println("Could not connect to any Wi-Fi network!");
  }
}