// Chytrý podtácek – test zapojení a kalibrace váhy
// 1) po startu se postupně rozsvítí LEDky (test pásku)
// 2) v Serial Monitoru (115200) se každou chvíli vypíše váha
// 3) příkazy do Serial Monitoru:  t = vynulovat,  k355 = kalibrace se závažím 355 g

#include "HX711.h"
#include <Adafruit_NeoPixel.h>

const int PIN_DT   = 4;   // HX711 DT
const int PIN_SCK  = 5;   // HX711 SCK
const int PIN_LED  = 3;   // DIN LED pásku
const int NUM_LEDS = 15;  // kolik LED máš na pásku (14 nebo 15)

HX711 scale;
Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);
float factor = 1.0;       // před kalibrací ukazuje "g" jen surová čísla

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Start testu...");

  strip.begin();
  strip.setBrightness(40);                      // nízký jas, šetří USB
  for (int i = 0; i < NUM_LEDS; i++) {          // LEDky se rozsvítí jedna po druhé
    strip.setPixelColor(i, strip.Color(0, 60, 150));
    strip.show();
    delay(80);
  }

  scale.begin(PIN_DT, PIN_SCK);
  if (!scale.wait_ready_timeout(2000)) {
    Serial.println("HX711 neodpovida – zkontroluj drate VCC, GND, DT, SCK");
  }
  scale.set_scale(factor);
  scale.tare(20);                               // podtácek musí být při startu prázdný
  Serial.println("Hotovo, prazdny podtacek = 0.");
  Serial.println("Prikazy: t = vynulovat, k<gramy> = kalibrace (napr. k355)");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 't') {
      scale.tare(20);
      Serial.println("Vynulovano.");
    } else if (c == 'k') {
      float grams = Serial.parseFloat();
      if (grams > 0) {
        float raw = scale.get_value(20);
        factor = raw / grams;
        scale.set_scale(factor);
        Serial.printf("SCALE = %.4f   <- tohle cislo posli Claudovi\n", factor);
      }
    }
  }

  if (scale.wait_ready_timeout(500)) {
    Serial.printf("vaha: %.1f g\n", scale.get_units(5));
  }
  delay(300);
}
