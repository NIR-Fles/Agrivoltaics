#include "GyverMAX6675.h"
#define DT 8
#define CK 9
#define SS 10
GyverMAX6675<CK, DT, SS> sensor;
float t = 0;
void setup() {
  Serial.begin(9600);
  delay(1000);
}
void loop() {
  if (sensor.readTemp()) t = sensor.getTemp();
  Serial.println(t);
  delay(1000);
}