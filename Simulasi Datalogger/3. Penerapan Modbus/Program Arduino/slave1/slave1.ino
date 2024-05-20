#include "ModbusRTU.h"
#include "GyverNTC.h"
#include "NewPing.h"
GyverNTC therm(1, 10000, 3435, 25, 10000);
ModbusRTU mb;
#define TRG 8
#define ECH 9
#define MAKS 200
NewPing sonar(TRG, ECH, MAKS);
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  mb.begin(&Serial);
  mb.setBaudrate(9600);
  mb.slave(1);    //slave ID
  mb.addHreg(0);  //sensor1
  mb.addHreg(1);  //sensor2
  mb.addHreg(2);  //sensor3
  delay(1000);
}
void loop() {
  if (millis() - skr > 1000) {
    float ldr = analogRead(A0) * 5.0 / 1023.0 * 100;
    mb.Hreg(0, int(ldr));
    float ntc = therm.getTempAverage() * 10;
    mb.Hreg(1, int(ntc));
    float srf = sonar.ping_cm() * 10;
    mb.Hreg(2, int(srf));
    skr = millis();
  }
  mb.task();
}