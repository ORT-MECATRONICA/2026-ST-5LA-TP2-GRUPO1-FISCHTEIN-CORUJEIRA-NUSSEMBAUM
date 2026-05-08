//Grupo 1: Sol Corujeira, Benjamín Fischtein y Lara Nussembaum

#include <WiFi.h>
#include <ESP32Time.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

ESP32Time rtc;
const char* ssid = "MECA-IoT";
const char* password = "IoT$2026";

void imprimirHora(int hora, int temperatura);

#define DHTPIN 23  // Pin del sensor de temperatura
#define DHTTYPE DHT11
#define LED 25
#define BOTON_1 35
#define BOTON_2 34

enum Estados {
  PANTALLA_1,
  ESPERA_1,
  PANTALLA_2,
  SUMA_GMT,
  RESTA_GMT,
  ESPERA_2
};
Estados estado = PANTALLA_1;
int gmt = 0;
DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  configTime(gmt, 0, "pool.ntp.org");

  // 3. Wait for time to sync and set it to the RTC
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);  // Set the internal RTC with the NTP time
  }

  pinMode(BOTON_1, INPUT);
  pinMode(BOTON_2, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println(F("OLED test"));
  u8g2.begin();
  u8g2.clearBuffer();
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("Pantalla 1");
}

void loop() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  configTime(gmt,0,"pool.ntp.org");
  int h = rtc.getHour("%H") ;
  imprimirHora(h, event.temperature);
  switch (estado) {
    case PANTALLA_1:
      if (digitalRead(BOTON_1) == LOW && digitalRead(BOTON_2) == LOW) {
        estado = ESPERA_1;
        Serial.println("Espera 1");
      }
      break;
    case ESPERA_1:
      if (digitalRead(BOTON_1) == HIGH && digitalRead(BOTON_2) == HIGH) {
        estado = PANTALLA_2;
        Serial.println("Pantalla 2");
      }
      break;

    case PANTALLA_2:
      if (digitalRead(BOTON_1) == LOW) {
        estado = SUMA_GMT;
        Serial.println("Suma gmt");
      } else if (digitalRead(BOTON_2) == LOW) {
        estado = RESTA_GMT;
        Serial.println("Resta gmt");
      }
      break;

    case SUMA_GMT:
      if (digitalRead(BOTON_2) == LOW) {
        estado = ESPERA_2;
        Serial.println("Espera 2");
      }
      if (digitalRead(BOTON_1) == HIGH) {
        estado = PANTALLA_2;
        gmt++;
        gmt = constrain(gmt, -12, 12);
        Serial.println("Pantalla 2");
      }
      break;

    case RESTA_GMT:
      if (digitalRead(BOTON_1) == LOW) {
        estado = ESPERA_2;
        Serial.println("Espera 2");
      }
      if (digitalRead(BOTON_2) == HIGH) {
        gmt--;
        gmt = constrain(gmt, -12, 12);
        estado = PANTALLA_2;
        Serial.println("Pantalla 2");
      }
      break;

    case ESPERA_2:
      if (digitalRead(BOTON_1) == HIGH && digitalRead(BOTON_2) == HIGH) {
        estado = PANTALLA_1;
        Serial.println("Pantalla 1");
      }
      break;
  }
}

void imprimirHora(int hora, int temperatura) {
  char shora[9];
  char stemp[6];
  char sgmt[4];

  u8g2.setFont(u8g2_font_6x10_tr);

  if (estado == PANTALLA_1) {
    u8g2.clearBuffer();
    u8g2.drawStr(10, 30, "Hora: ");
    sprintf(shora, "%s", String(hora) + rtc.getTime(":%M:%S"));
    u8g2.drawStr(46, 30, shora);

    sprintf(stemp, "%d", temperatura);
    u8g2.drawStr(10, 50, "Temperatura:");
    u8g2.drawStr(85, 50, stemp);
    u8g2.drawStr(100, 50, "°C");
  } else if (estado != ESPERA_1) {
    u8g2.clearBuffer();
    sprintf(sgmt, "%d", gmt);
    u8g2.drawStr(10, 30, "GMT: ");
    u8g2.drawStr(40, 30, sgmt);
  }

  u8g2.sendBuffer();
}