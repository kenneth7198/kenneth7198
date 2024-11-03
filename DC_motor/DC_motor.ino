int dcMotorPin = 6;
int ledPin = 5;
void setup() { 
  pinMode(dcMotorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); 
} 
 
void loop() {
   digitalWrite(dcMotorPin, HIGH);
   digitalWrite(ledPin, HIGH);
   Serial.println("ON");
   delay(1000);
   digitalWrite(dcMotorPin, LOW);
   digitalWrite(ledPin, LOW);
   Serial.println("OFF");
   delay(5000);
}