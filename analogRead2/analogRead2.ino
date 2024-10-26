int readAnalog;
int analogPin = 0;
void setup() {
 Serial.begin(9600);
}
 
void loop() {
  readAnalog = analogRead(analogPin);
  if(readAnalog < 100 || readAnalog > 900){
    Serial.println("ON");
}else{
    Serial.print("Aanlog Input:");
    Serial.println(readAnalog);
}
  delay(100);
}