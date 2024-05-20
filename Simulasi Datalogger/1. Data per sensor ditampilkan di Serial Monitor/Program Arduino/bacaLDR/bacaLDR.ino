void setup() {
  Serial.begin(9600);
  delay(1000);
}
void loop() {
  float ldr = analogRead(A0) * 5 / 1023.0;
  Serial.println(ldr);
  delay(1000);
}