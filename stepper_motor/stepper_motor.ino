#include <Stepper.h>
int stepsRevolution  = 2048;
Stepper myStepper(stepsRevolution, 8,10,11,9);
 
void setup() {
  myStepper.setSpeed(5);
  Serial.begin(9600);
}
void loop() {
  Serial.println("clockWise");
  myStepper.step(stepsRevolution);
  delay(500);
 
  Serial.println("counterClockWise");
  myStepper.step(-stepsRevolution);
  delay(500);
}