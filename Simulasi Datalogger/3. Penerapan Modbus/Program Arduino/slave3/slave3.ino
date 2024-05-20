#include "ModbusRTU.h"
#include "DHT.h"
#include "sht_7x.h"
#include "TH02.h"
#include "Wire.h"
DHT dht(8, DHT11);
SHT_7x sht(9, 10);  //(sck,data)
TH02 th02(0x40);
ModbusRTU mb;
int16_t temp, rh;
unsigned long skr = 0;
void setup() {
  Serial.begin(9600);
  sht.init();
  dht.begin();
  Wire.begin();
  mb.begin(&Serial);
  mb.setBaudrate(9600);
  mb.slave(3);    //slave ID
  mb.addHreg(0);  //sensor1a
  mb.addHreg(1);  //sensor1b
  mb.addHreg(2);  //sensor2a
  mb.addHreg(3);  //sensor2b
  mb.addHreg(4);  //sensor3a
  mb.addHreg(5);  //sensor3b
  delay(1000);
  Serial.println();
}
void loop() {
  if (millis() - skr > 1000) {
    float t1 = dht.readTemperature() * 10;
    float h1 = dht.readHumidity() * 10;
    mb.Hreg(0, int(t1));
    mb.Hreg(1, int(h1));
    float t2 = sht.getTemperature() * 10;
    float h2 = sht.getHumidity() * 10;
    mb.Hreg(2, int(t2));
    mb.Hreg(3, int(h2));
    th02.startTempConv();
    delay(50);
    temp = th02.getConversionValue();
    float t3 = th02.getLastRawTemp() / 100.0 * 10;
    mb.Hreg(4, int(t3));
    th02.startRHConv();
    delay(50);
    rh = th02.getConversionValue();
    float h3 = th02.getLastRawRH() / 100.0 * 10;
    mb.Hreg(5, int(h3));
    skr = millis();
  }
  mb.task();
}