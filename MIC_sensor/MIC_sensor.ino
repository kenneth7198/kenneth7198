int snd = 0;
void setup(){
  Serial.begin(9600);  
}

void loop(){
  snd = analogRead(0);
  Serial.println(snd);
  delay(20);  
}
