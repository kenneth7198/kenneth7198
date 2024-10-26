int readAnalog;
int analogPin = 0;
void setup() {
 Serial.begin(9600);
}

void loop() {
  readAnalog = analogRead(analogPin);
  Serial.print("Aanlog Input:");
  Serial.println(readAnalog);
  delay(100);
}
