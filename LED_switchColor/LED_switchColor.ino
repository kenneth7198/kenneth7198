#include <FastLED.h>
#define NUM_LEDS 15
#define DATA_PIN 3
   
CRGB leds[NUM_LEDS];
   
void setup() {
    delay(2000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
}
void loop() {
   for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0,255,0);   
      FastLED.show();
      delay(500);    
   }
   
   for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0,0,255);   
      FastLED.show();
      delay(500);    
   }
}