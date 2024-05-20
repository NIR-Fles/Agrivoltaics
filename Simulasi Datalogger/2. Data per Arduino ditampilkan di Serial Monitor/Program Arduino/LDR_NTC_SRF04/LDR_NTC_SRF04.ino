#include <GyverNTC.h>
#include <NewPing.h>
GyverNTC therm(1, 10000, 3435, 25, 10000);
#define TRG 8
#define ECH 9
#define MAKS 200
NewPing sonar(TRG, ECH, MAKS);
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  delay(1000);
}
void loop() {
  if (millis() - skr > 3000) {
    float ldr = analogRead(A0) * 5 / 1023.0 * 10;
    Serial.print(ldr);
    Serial.print(',');
    Serial.print(therm.getTempAverage());
    Serial.print(',');
    Serial.println(sonar.ping_cm());
    skr = millis();
  }
}