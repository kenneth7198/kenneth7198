int interval = 150;
int speakerPin = 11;
int adcPin = 0;
int val = 0;
void setup() {
  pinMode(speakerPin, OUTPUT);
  Serial.begin(9600);
}
  
void loop() {
  int input = analogRead(adcPin);
  val = map(input, 0, 1023, 262, 1976);
  tone(speakerPin, val, interval);
  Serial.println(val);
  delay(interval);
}