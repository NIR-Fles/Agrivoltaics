#include <SPI.h>
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
int val = 0;
unsigned long skr = 0;
float t = 0;
double c = 0;
void setup() {
  Serial.begin(9600);
  delay(2000);
}
void loop() {
  if (millis() - skr > 3000) {
    if (sensor.readTemp()) t = sensor.getTemp();
    Serial.print(t);
    Serial.print(',');
    c = thermo.readCelsius();
    Serial.print(c);
    Serial.print(',');
    int a = analogRead(A0);
    float p = ((a * .00481 / 5) + .094) / .009;
    Serial.println(p);
    skr = millis();
  }
}