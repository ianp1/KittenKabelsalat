#include <Arduino.h>
#include "Connector.h"

#include "driver/rtc_io.h"

#define FASTLED_RGBW
#include <FastLED.h>
#include "FastLED_RGBW.h"

#include "introducion.h"// Die Anleitung für deine Seite
#include <Wire.h>       // Die Libary für die Verbindung

#define LED_BUILTIN 2   // Onbord LED, fals du sie benutzen möchtest

// Stati, die du seten kannst um dem Master Dinge mitzuteilen.
#define STATUS_NO_INIT 254  // Ich bin noch nicht bereit, bitte initialisiere mich
#define STATUS_OK 0         // Alles okay, alles läuft wie geplant
#define STATUS_WIN 200      // Meine Seite wurde Erfolgreich geschafft

#define NUM_LEDS 10
#define LED_BAR_PIN 23

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
#define CONNECTOR_COUNT 10

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

Connector connectors[CONNECTOR_COUNT] = {
  Connector(GPIO_NUM_36, 0),
  Connector(GPIO_NUM_13, 1),
  Connector(GPIO_NUM_34, 2),
  Connector(GPIO_NUM_33, 3),
  Connector(GPIO_NUM_35, 4),
  Connector(GPIO_NUM_26, 5),
  Connector(GPIO_NUM_4, 6),
  Connector(GPIO_NUM_32, 7),
  Connector(GPIO_NUM_25, 8),
  Connector(GPIO_NUM_27, 9)
};

void setup() {
  Serial.begin(115200);
  initMasterConnection();


	FastLED.addLeds<UCS1903, LED_BAR_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  
  FastLED.setBrightness(200);
  leds[0] = CRGB::White;
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Green;
  leds[3] = CRGB(60, 60, 240);//Hellblau
  leds[4] = CRGB::Turquoise;
  leds[5] = CRGB(255, 92, 113);//pink
  leds[6] = CRGB::Orange;
  leds[7] = CRGB::Yellow;
  leds[8] = CRGB(0, 0, 250);//Dunkelblau
  leds[9] = CRGB::Purple;
  FastLED.show();
  delay(1000);
}

void loop() {
  delay(10);
  digitalWrite(LED_BUILTIN, HIGH);

  for (int i = 0; i < CONNECTOR_COUNT; i++) {
    connectors[i].loop();
  }

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
  }
}
