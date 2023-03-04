#include <Arduino.h>

#include "driver/rtc_io.h"

#include "introducion.h"// Die Anleitung für deine Seite
#include <Wire.h>       // Die Libary für die Verbindung
#define LED_BUILTIN 2   // Onbord LED, fals du sie benutzen möchtest

// Stati, die du seten kannst um dem Master Dinge mitzuteilen.
#define STATUS_NO_INIT 254  // Ich bin noch nicht bereit, bitte initialisiere mich
#define STATUS_OK 0         // Alles okay, alles läuft wie geplant
#define STATUS_WIN 200      // Meine Seite wurde Erfolgreich geschafft
//Alles höhere ist der Fehler counter
volatile byte statusChar = STATUS_NO_INIT; // Am anfang nicht bereit

// Das end Byte wird gesetzt, sobald sich der Spielstatus verändert, 
#define END_IDLE 0
#define END_RUNNING 1
#define END_LOST 2
#define END_WIN 3
volatile byte endByte = END_IDLE;// 0 = Nichts, 1 = läuft, 2 = Verloren, 3 = Gewonnen

volatile int seed = 0;    // Der Seed wird bei der Initialisierung übertragen und soll verschiedene Wege eröffnen

#include "MasterCommunication.h"

//Number of pins to be used for cable connectors
#define CONNECTOR_PIN_COUNT 10
//Number of consecutive readings that have to be the same in order to qualify as being stable
#define CONNECTOR_STABLE_READINGS_NUM 3
//Number of readings to be aggregated before using them
#define CONNECTOR_READ_MAX 5

//pins to be used for connectors
gpio_num_t connectorPins[CONNECTOR_PIN_COUNT] = {
  GPIO_NUM_27, 
  GPIO_NUM_25, 
  GPIO_NUM_32, 
  GPIO_NUM_4, 
  GPIO_NUM_26, 
  GPIO_NUM_35, 
  GPIO_NUM_33, 
  GPIO_NUM_34, 
  GPIO_NUM_13,//=tck pin 
  GPIO_NUM_36,//=svp pin
};

//aggregated values of adc pins for cable connectors
long connectorPinValues[CONNECTOR_PIN_COUNT];
//current evaluated value of pin
int connectorPinReading[CONNECTOR_PIN_COUNT];
//last evaluation of pin connections to check if reading is stable
int connectorPinLastReading[CONNECTOR_PIN_COUNT];
//count of equivalent readings on pins, used to determine stable state
int connectorPinReadingStableCount[CONNECTOR_PIN_COUNT];
//stable state of all pins
bool connectorPinStable[CONNECTOR_PIN_COUNT];

//number of aggregated measurements read on connector pins
int connectorReadCount = 0;

void setup() {
  //initMasterConnection();

  for (int i = 0; i < CONNECTOR_PIN_COUNT; i++) {
    connectorPinValues[i] = 0;
    connectorPinReading[i] = 0;
    connectorPinLastReading[i] = 0;
  }

  Serial.begin(115200);// Optional um dinge auszugeben, beim Debuggen
}

void loop() {
  delay(10);
  digitalWrite(LED_BUILTIN, HIGH);

  //Read all adc inputs for cable connectors
  for (int i = 0; i < CONNECTOR_PIN_COUNT; i++) {
    connectorPinValues[i] += analogRead(connectorPins[i]);
  }
  connectorReadCount++;

  if (connectorReadCount > CONNECTOR_READ_MAX) {
    for (int i = 0; i < CONNECTOR_PIN_COUNT; i++) {
      connectorPinLastReading[i] = connectorPinReading[i];
    }

    for (int i = 0; i < CONNECTOR_PIN_COUNT; i++) {
      double currentValue = ((connectorPinValues[i]/(double)(connectorReadCount)) - 150.0) / 302.0;
      /*
      if (i == 0) {
        Serial.print("current value: ");
        Serial.println(currentValue);
        Serial.println(connectorPinValues[i]);
      }*/

      if (currentValue > -0.03) {
        /*Serial.print("PIN ");
        Serial.print(i + 1);
        Serial.print(" connected to connector ");
        Serial.println((int)round(currentValue) + 1);*/
        connectorPinReading[i] = (int)round(currentValue);
      } else {
        connectorPinReading[i] = -1;
      }

      connectorPinValues[i] = 0;
    }

    for (int i = 0; i < CONNECTOR_PIN_COUNT; i++) {
      if (connectorPinLastReading[i] == connectorPinReading[i]) {
        connectorPinReadingStableCount[i] ++;
        if (connectorPinReadingStableCount[i] >= 3) {
          connectorPinReadingStableCount[i] = 3;
          connectorPinStable[i] = true;

          if (connectorPinReading[i] >= 0) {
            Serial.print("stable reading: ");
            Serial.print(i + 1);
            Serial.print(" connected to connector ");
            Serial.println(connectorPinReading[i] + 1);
          }
        } else {
          connectorPinStable[i] = false;
        }
      }
    }

    connectorReadCount = 0;
  }
  /*
  while (endByte == END_RUNNING) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    statusChar = 25;

    

    // Aktionen machen und alles was du möchtest
    // Code am besten non Blocking (max laufzeit < 1000 ms)
  }
  while (endByte == END_LOST) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    // Verloren Animation anzeigen
  }
  while (endByte == END_WIN) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    // Win Animation anzeigen
  }*/
}
