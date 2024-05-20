#include <GyverNTC.h>
GyverNTC therm(0, 10000, 3435, 25, 10000);	
void setup() {
  Serial.begin(9600);
}
void loop() {
  Serial.print("Temperature ");
  Serial.print(therm.getTempAverage());
  Serial.println(" *C");
  delay(500);
}