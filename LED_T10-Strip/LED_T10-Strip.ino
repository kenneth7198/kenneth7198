int T10_LED_Pin = 6;
int ledPin = 5;
void setup() { 
  pinMode(T10_LED_Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); 
} 

void loop() {
   digitalWrite(T10_LED_Pin, HIGH);
   digitalWrite(ledPin, HIGH);
   Serial.println("ON");
   delay(1000);
   digitalWrite(T10_LED_Pin, LOW);
   digitalWrite(ledPin, LOW);
   Serial.println("OFF");
   delay(1000);
}
