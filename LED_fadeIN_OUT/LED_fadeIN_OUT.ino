#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6
   
CRGB leds[NUM_LEDS];
   
void setup() {
    delay(2000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
}
void loop() {
  for(int j = 0; j<256; j++){
   for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(j,0,0);  
   }
   FastLED.show();
   delay(25);
  }
   
  for(int j = 255; j>0; j--){
   for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0,j,0);  
   }
   FastLED.show();
   delay(25);
  }
}

