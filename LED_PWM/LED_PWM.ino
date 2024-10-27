int readAnalog;
int analogPin = 0;
int ledPin = 10;
void setup() {
 Serial.begin(9600);
 pinMode(ledPin, OUTPUT);
}

void loop() {
  readAnalog = analogRead(analogPin);
  Serial.print("Aanlog Input:");
  Serial.println(readAnalog);
  //運用LED的PWM輸出讓LED具備漸暗與漸亮效果
  //運用map函式來轉換原本輸入的類比訊號，轉換成PWM輸出0~255
  int pwm = map(readAnalog, 0, 1023, 0, 255); 
  //運用analogWrite函式控制LED的PWM輸出
  analogWrite(ledPin, pwm);
  delay(100);
}
