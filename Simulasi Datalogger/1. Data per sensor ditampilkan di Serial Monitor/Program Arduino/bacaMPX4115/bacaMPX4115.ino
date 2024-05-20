int pressurePin = 0;
int val;
float pkPa; 
void setup(){
  Serial.begin(9600); 
}
void loop(){  
  val = analogRead(pressurePin);
  pkPa = ((val * .00481 / 5) + .094) / .009;
  Serial.println(pkPa);
  delay(1000);
}