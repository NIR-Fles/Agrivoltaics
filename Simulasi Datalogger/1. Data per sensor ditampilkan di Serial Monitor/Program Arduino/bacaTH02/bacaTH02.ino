#include <Wire.h>
#include <TH02.h>
TH02 th02(0x40);
int16_t temp, rh;
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  Wire.begin();
}
void loop() {
  if (millis() - skr > 1000) {
    skr = millis();
    th02.startTempConv();
    delay(50);
    temp = th02.getConversionValue();
    temp = th02.getLastRawTemp() / 100.0;
    th02.startRHConv();
    delay(50);
    rh = th02.getConversionValue();
    rh = th02.getLastRawRH() / 100.0;
    Serial.print(temp);
    Serial.print(',');
    Serial.println(rh);
  }
}