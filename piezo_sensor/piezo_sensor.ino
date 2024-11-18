int val = 0;
int piezoPin = 0;
int ledPin = 6;
void setup(){
  Serial.begin(9600);  
  pinMode(ledPin,OUTPUT);
}

void loop(){
  val = analogRead(piezoPin);
  if(val > 300){
    Serial.println("ON");
  }else{
    Serial.println(val);
  }
  int mapVal = map(val,0,1023,0,255);
  analogWrite(ledPin,mapVal);
  delay(20);  
}
