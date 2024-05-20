#include "DHT.h"
#include "sht_7x.h"
#include "TH02.h"
#include "Wire.h"
DHT dht(8, DHT11);
SHT_7x sht(9, 10);
TH02 th02(0x40);
int16_t temp, rh;
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  sht.init();
  dht.begin();
  Wire.begin();
  Serial.println();
}
void loop() {
  if (millis() - skr > 1000) {
    float t1 = dht.readTemperature();
    float h1 = dht.readHumidity();
    Serial.print("DHT11: ");
    Serial.print(t1);
    Serial.print(',');
    Serial.println(h1);
    Serial.print("SHT75: ");
    float t2 = sht.getTemperature();
    float h2 = sht.getHumidity();
    Serial.print(t2);
    Serial.print(',');
    Serial.println(h2);
    th02.startTempConv();
    delay(50);
    temp = th02.getConversionValue();
    float t3 = th02.getLastRawTemp() / 100.0;
    th02.startRHConv();
    delay(50);
    rh = th02.getConversionValue();
    float h3 = th02.getLastRawRH() / 100.0;
    Serial.print("TH02 : ");
    Serial.print(t3);
    Serial.print(',');
    Serial.println(h3);
    skr = millis();
  }
}