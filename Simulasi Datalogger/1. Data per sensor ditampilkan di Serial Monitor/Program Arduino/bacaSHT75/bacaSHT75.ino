#include <sht_7x.h>
SHT_7x sht(9, 10);
void setup() {
  Serial.begin(9600);
  sht.init();
  Serial.println();
}
void loop() {
  float t2 = sht.getTemperature();
  float h2 = sht.getHumidity();
  Serial.print(t2);
  Serial.print(',');
  Serial.println(h2);
  delay(2000);
}