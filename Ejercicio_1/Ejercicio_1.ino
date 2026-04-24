//Grupo 3: Sol Corujeira, Benjamín Fischtein y Lara Nussembaum

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void imprimirHora(int hora, int minuto);
void imprimirTemp(int temperatura);

#define DHTPIN 23  // Pin del sensor de temperatura
#define DHTTYPE DHT11
#define LED 25
#define BOTON_1 35
#define BOTON_2 34

enum Estados {
  PANTALLA_1,
  ESPERA_1,
  PANTALLA_2,
  SUMA_HORA,
  SUMA_MINUTO,
  ESPERA_2
};
Estados estado = PANTALLA_1;
int h = 10;
int m = 20;
int segundo = 0;

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(BOTON_1, INPUT_PULLUP);
  pinMode(BOTON_2, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(9600);
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
  switch (estado) {
    case PANTALLA_1:
      imprimirHora(h, m, event.temperature);
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
      imprimirHora(h, m, event.temperature);
      if (digitalRead(BOTON_1) == LOW) {
        estado = SUMA_HORA;
        Serial.println("Suma Hora");
      } else if (digitalRead(BOTON_2) == LOW) {
        estado = SUMA_MINUTO;
        Serial.println("Suma Minuto");
      }
      break;

    case SUMA_HORA:
      if (digitalRead(BOTON_2) == LOW) {
        estado = ESPERA_2;
        Serial.println("Espera 2");
      }
      if (digitalRead(BOTON_1) == HIGH) {
        estado = PANTALLA_2;
        h++;
        Serial.println("Pantalla 2");
      }
      break;

    case SUMA_MINUTO:
      if (digitalRead(BOTON_1) == LOW) {
        estado = ESPERA_2;
        Serial.println("Espera 2");
      }
      if (digitalRead(BOTON_2) == HIGH) {
        m++;
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

  if (millis() % 1000 == 0) {
    segundo++;
  }
  if (segundo >= 60) {
    segundo = 0;
    m++;
  }
  if (m >= 60) {
    m = 0;
    h++;
  }
  if (h >= 24) {
    h = 0;
  }
}

void imprimirHora(int hora, int minuto, int temperatura) {
  u8g2.clearBuffer();  // clear the internal memory
  char shora[2];
  char smin[2];
  char stemp[2];

  u8g2.setFont(u8g2_font_6x10_tr);

  if (hora < 10) {
    sprintf(shora, "0%d", hora);
  } else {
    sprintf(shora, "%d", hora);
  }
  u8g2.drawStr(46, 30, shora);

  u8g2.drawStr(60, 30, ":");

  if (minuto < 10) {
    sprintf(smin, "0%d", minuto);
  } else {
    sprintf(smin, "%d", minuto);
  }
  u8g2.drawStr(66, 30, smin);

  if (estado != PANTALLA_2) {
    u8g2.drawStr(10, 30, "Hora: ");
    sprintf(stemp, "%d", temperatura);
    u8g2.drawStr(10, 50, "Temperatura:");
    u8g2.drawStr(85, 50, stemp);
    u8g2.drawStr(100, 50, "°C");
  }

  u8g2.sendBuffer();
}