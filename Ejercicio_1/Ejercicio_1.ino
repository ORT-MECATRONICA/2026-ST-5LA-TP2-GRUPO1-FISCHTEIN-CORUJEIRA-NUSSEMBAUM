//Grupo 1: Sol Corujeira, Benjamín Fischtein y Lara Nussembaum

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
int hora = 10;
int minuto = 20;
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
      imprimirHora(hora,minuto,event.temperature);
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
      break;

    case SUMA_HORA:
      break;

    case SUMA_MINUTO:
      break;

    case ESPERA_2:
      break;
  }

  if(millis() % 1000 == 0){
    
  }
}

void imprimirHora(int hora, int minuto, int temperatura) {
  u8g2.clearBuffer();  // clear the internal memory
  char shora[2];
  char smin[2];
  char stemp[2];

  sprintf(shora, "%d", hora);
  sprintf(smin, "%d", minuto);
  sprintf(stemp, "%d", temperatura);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 30, "Hora: ");
  u8g2.drawStr(16,30,shora);
  u8g2.drawStr(18, 30, ":");
  u8g2.drawStr(20, 30, smin);
  u8g2.sendBuffer();
}