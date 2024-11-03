int PIR_Pin = 8;
int ledPin = 10;
int val = 0;
void setup() {
  Serial.begin(9600);
  pinMode(PIR_Pin, INPUT);
  pinMode(ledPin, OUTPUT);
}
void loop() {
  val = digitalRead(PIR_Pin);
  Serial.println(val);
  if(val == 0){
    Serial.println("No motion");  //沒人靠近
    digitalWrite(ledPin, LOW);
  }else{
    Serial.println("Motion");   //有人靠近
    digitalWrite(ledPin, HIGH);
  }
  delay(1000);
}