#include <SPI.h>
#include "Adafruit_MAX31855.h"
#define DO 8
#define CLK 9
#define CS 10
Adafruit_MAX31855 thermocouple(CLK, CS, DO);
void setup() {
  Serial.begin(9600);
  delay(500);
}
void loop() {
  double c = thermocouple.readCelsius();
  Serial.println(c);
  delay(1000);
}