#include <dht11.h>
dht11 DHT11;
byte dataPin = 2;
float h = 0.0;
float t = 0.0;
void setup(){
  Serial.begin(9600);    
}
void loop(){
  int dataIn = DHT11.read(dataPin);
    if(dataIn == 0){
      h = DHT11.humidity;
      t = DHT11.temperature;
      Serial.print("Humidity %:");
      Serial.println(h);
      Serial.print("Temperature:");
      Serial.println(t);
    }  
   delay(2000); 
}
