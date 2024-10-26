int readAnalog;
int analogPin = 0;
int ledPin = 9;
void setup() {
 Serial.begin(9600);
 pinMode(ledPin, OUTPUT);
}
 
void loop() {
  readAnalog = analogRead(analogPin);
  //將擷取到的類比訊號用map函式轉換成LED的PWM輸出
  int pwm = map(readAnalog, 0, 1023, 0, 255);
  analogWrite(ledPin, pwm);
  delay(100);
}