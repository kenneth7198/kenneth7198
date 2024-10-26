#include <FastLED.h>
#define NUM_LEDS 30
#define DATA_PIN 6
   
CRGB leds[NUM_LEDS];
   
void setup() {
    delay(2000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
}
void loop() {
   for(int i = 0; i < NUM_LEDS; i++) {
//      leds[i].r = 0;
//      leds[i].g = 0;
//      leds[i].b = 255;
      //leds[i] = 0xFF44DD;
      leds[i] = CRGB(50, 100, 0);
       
      FastLED.show();
      delay(25);    
   }
}