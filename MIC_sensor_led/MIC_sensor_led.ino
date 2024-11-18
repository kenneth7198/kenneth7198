int snd = 0;
int led = 6;
void setup(){
  Serial.begin(9600);  
  pinMode(led, OUTPUT);
}

void loop(){
  snd = analogRead(0);
  Serial.println(snd);
  int mapVal = map(snd, 0, 1023, 0, 255);
  analogWrite(led,mapVal);
  if(snd > 600){
    Serial.println("ON");
    analogWrite(led, 255);
  }
  delay(20);  
}
