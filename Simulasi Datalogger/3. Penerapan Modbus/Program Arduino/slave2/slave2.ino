#include "ModbusRTU.h"
#include "SPI.h"
#include "GyverMAX6675.h"
#include "Adafruit_MAX31855.h"
#define DT 8
#define CK 9
#define SS 10
#define CLK 11
#define DO 12
#define CS 13
GyverMAX6675<CK, DT, SS> sensor;
Adafruit_MAX31855 thermo(CLK, CS, DO);
ModbusRTU mb;
float t = 0;
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  mb.begin(&Serial);
  mb.setBaudrate(9600);
  mb.slave(2);    //slave ID
  mb.addHreg(0);  //sensor1
  mb.addHreg(1);  //sensor2
  mb.addHreg(2);  //sensor3
  delay(1000);
}
void loop() {
  if (millis() - skr > 1000) {
    if (sensor.readTemp()) t = sensor.getTemp() * 10;
    mb.Hreg(0, int(t));
    float c = thermo.readCelsius() * 10;
    mb.Hreg(1, int(c));
    int a = analogRead(A0);
    float p = ((a * .00481 / 5) + .094) / .009 * 10;
    mb.Hreg(2, int(p));
    skr = millis();
  }
  mb.task();
}