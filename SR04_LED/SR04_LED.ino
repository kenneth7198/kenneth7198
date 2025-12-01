int trigPin = 12;
int echoPin = 11;
int ledPin = 10;
void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
}
void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.4;
  if(distance >= 300 ){
    Serial.println("Out of range");  
  }else{
    Serial.print(distance);  
    Serial.println(" cm");
    //讓LED亮滅
    if(distance < 100){  //若小於100cm, LED亮起
    	digitalWrite(ledPin, HIGH);
    }else{   //反之不亮
    	digitalWrite(ledPin, LOW);
    }
  }
  delay(100);
}
