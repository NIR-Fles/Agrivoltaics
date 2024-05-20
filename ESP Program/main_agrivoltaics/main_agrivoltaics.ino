#include <Wire.h>
#include "ClosedCube_SHT31D.h"

ClosedCube_SHT31D sht3xd;

float temperature, humidity;

void setup()
{
	Wire.begin(); // Default SDA & SCL is GPIO 8 and GPIO 9
	Serial.begin(115200);
	sht3xd.begin(0x44); // I2C address: 0x44 or 0x45
	if (sht3xd.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ) != SHT3XD_NO_ERROR) // SHT31-D Error Checking
  { 
		Serial.println("[ERROR] Cannot start periodic mode");
  }
}

void loop()
{
	printResult(sht3xd.periodicFetchData());
	delay(1000);
}

void printResult(SHT31D result) {
	if (result.error == SHT3XD_NO_ERROR) {
    temperature = result.t;
    humidity = result.rh;
		Serial.print("Temperature = ");
		Serial.print(temperature);
		Serial.print(" °C,   Humidity = ");
		Serial.print(humidity);
		Serial.println(" %RH");
	} else {
		Serial.print("[ERROR] Code #");
		Serial.println(result.error);
	}
}
