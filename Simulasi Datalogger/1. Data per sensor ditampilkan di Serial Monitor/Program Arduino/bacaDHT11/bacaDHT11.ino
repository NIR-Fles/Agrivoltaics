#include "DHT.h"
unsigned long skr = 0;
DHT dht(8, DHT11);
void setup() {
  Serial.begin(9600);
  dht.begin();
}
void loop() {
  if (millis() - skr > 1000) {
    skr = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.print(h);
    Serial.print(',');
    Serial.println(t);
  }
}