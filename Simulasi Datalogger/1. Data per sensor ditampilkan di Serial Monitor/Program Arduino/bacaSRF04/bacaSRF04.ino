#include <NewPing.h>
#define TRG 8
#define ECH 9
#define MAKS 200
NewPing sonar(TRG, ECH, MAKS);
void setup() {
  Serial.begin(9600);
  delay(1000);
}
void loop() {
  Serial.println(sonar.ping_cm());
  delay(1000);
}