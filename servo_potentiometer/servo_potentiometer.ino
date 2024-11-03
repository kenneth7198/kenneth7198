#include <Servo.h>
Servo myservo; 
void setup() { 
  myservo.attach(3);
  Serial.begin(9600); 
} 
 
void loop() {
   int val = analogRead(0);
   int mapVal = map(val, 0, 1023, 0, 180);
   Serial.println(mapVal);
   myservo.write(mapVal); 
   delay(20);
}