#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6
   
int reading;
int analogPin = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
const long intervalA = 100;
const long intervalB = 50;
   
CRGB leds[NUM_LEDS];
   
void setup() {
    Serial.begin(9600);
    delay(2000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
}
void loop() {
 unsigned long currentMillis1 = millis();
 unsigned long currentMillis2 = millis();
   
 reading = analogRead(analogPin);
 if(currentMillis1 - previousMillis1 >= intervalA){
  previousMillis1 = currentMillis1;
  Serial.print("ADC:");
  Serial.println(reading);
 }
   
 if(currentMillis2 - previousMillis2 >= intervalB){
  previousMillis2 = currentMillis2;
  int ledCtrl = map(reading, 0, 1023, 0, 60);
    for(int i=0;i < ledCtrl ; i++){
      leds[i] = CRGB(0, 120, 200);
    }
    FastLED.show();
    FastLED.clear();
  }
}