#include <WiFi.h>
#include <ESP32Time.h> // Download from Library Manager (by Felix Biego)

const char* ssid     = "MECA-IoT";
const char* password = "IoT$2026";

ESP32Time rtc; // Create an instance of ESP32Time
int gmt = -3;
void setup() {
  Serial.begin(115200);

  // 1. Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  // 2. Initialize NTP (configTime is a built-in ESP32 function)
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer)
  configTime(0, 0, "pool.ntp.org"); 

  // 3. Wait for time to sync and set it to the RTC
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo); // Set the internal RTC with the NTP time
  }
}

void loop() {
  // 4. Retrieve and print time directly from the RTC
  // This continues to work even if Wi-Fi is disconnected
  int hora = rtc.getHour("%H") + gmt;
  Serial.print("Hora: ");
  Serial.print(hora);
  Serial.println(rtc.getTime(":%M")); 
  delay(1000);
}
