#include "ClosedCube_SHT31D.h"
#include <Wire.h>
#include "Nextion.h" // must change the library itself called NexUpload.cpp from SoftwareSerial.h to HardwareSerial.h due to esp32 compatibility [fixed]
#include <RMaker.h>
#include <WiFi.h> // error occured not found bits\error_constants.h -> must copy from other related file [fixed]
#include <WiFiProv.h>
#include <AppInsights.h> // error occured when try to compile gen_insight_packages.py -> gtw bjir tiba tiba bisa gitu [fixed]
#include "SimpleTimer.h"

ClosedCube_SHT31D sht3xd;
SimpleTimer Timer;

NexDSButton bt0 = NexDSButton(1, 5, "bt0");
NexDSButton bt1 = NexDSButton(1, 6, "bt1");
NexDSButton bt2 = NexDSButton(1, 7, "bt2");
NexDSButton bt3 = NexDSButton(1, 8, "bt3");

NexTouch *nex_listen_list[] = 
{
    &bt0,
    &bt1,
    &bt2,
    &bt3,
    NULL
};

const char *service_name = "Agrivoltaics";
const char *pop = "Agrivoltaics2024";
static uint8_t gpio_reset = 0; // GPIO 0 
static uint8_t gpio_switch0 = 4; // GPIO 4
static uint8_t gpio_switch1 = 5; // GPIO 5
static uint8_t gpio_switch2 = 6; // GPIO 6
static uint8_t gpio_switch3 = 7; // GPIO 7  
static TemperatureSensor *my_temperature = NULL; // can also initially set the value like bellow this
static TemperatureSensor *my_humidity = NULL;    // static 'TYPE_OF_YOUR_COMPONENT' 'NAME_OF_YOUR_COMPONENT_AS_VARIABLE'("DEVICE_PARAMETER", '&RELATED_GPIO_PIN');
static TemperatureSensor *my_soil_humidity = NULL;
static TemperatureSensor *my_soil_temperature = NULL;
static TemperatureSensor *my_soil_conductivity = NULL;
static TemperatureSensor *my_soil_ph = NULL;
static TemperatureSensor *my_soil_nitrogen = NULL;
static TemperatureSensor *my_soil_phosphorus = NULL;
static TemperatureSensor *my_soil_potassium = NULL;
static Switch *my_switch0 = NULL;                // ignore ''
static Switch *my_switch1 = NULL;                // TYPE_OF_YOUR_COMPONENT is like builtin feature like 'Switch' , 'fan' , 'TemperatureSensor' ,  etc
static Switch *my_switch2 = NULL;                // NAME_OF_YOUR_COMPONENT_AS_VARIABLE is whatever you want to naming it to be known by system as variable
static Switch *my_switch3 = NULL;                // DEVICE_PARAMETER is related to what TYPE_OF_YOUR_COMPONENT did you used and it's builtin feature 
bool switch0_state = false;
bool switch1_state = false;
bool switch2_state = false;
bool switch3_state = false;

unsigned long previousMillis = 0;
const long interval = 1000; // Interval in milliseconds

float temperature, humidity;
unsigned int soilHumidity, soilTemperature,soilConductivity, soilPH, nitrogen, phosphorus, potassium;
// int temp_nextion, hum_nextion;

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
      break;
    case ARDUINO_EVENT_PROV_INIT:
      wifi_prov_mgr_disable_auto_stop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      wifi_prov_mgr_stop_provisioning();
      break;
    default:;
    }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();
  if (strcmp(device_name, "Switch Pump 1") == 0) 
  {
    // Serial.printf("Lightbulb = %s\n", val.val.b ? "true" : "false");
    if (strcmp(param_name, "Power") == 0) 
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch0_state = val.val.b;
      (switch0_state == false) ? digitalWrite(gpio_switch0, LOW) : digitalWrite(gpio_switch0, HIGH);
      param->updateAndReport(val);
    }
  }
  else if (strcmp(device_name, "Switch Pump 2") == 0) 
  {
    if (strcmp(param_name, "Power") == 0) 
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch1_state = val.val.b;
      (switch1_state == false) ? digitalWrite(gpio_switch1, LOW) : digitalWrite(gpio_switch1, HIGH);
      param->updateAndReport(val);
    }
  }
  else if (strcmp(device_name, "Switch Valve 1") == 0) 
  {
    if (strcmp(param_name, "Power") == 0) 
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch2_state = val.val.b;
      (switch2_state == false) ? digitalWrite(gpio_switch2, LOW) : digitalWrite(gpio_switch2, HIGH);
      param->updateAndReport(val);
    }
  }
  else if (strcmp(device_name, "Switch Valve 2") == 0) 
  {
    if (strcmp(param_name, "Power") == 0) 
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch3_state = val.val.b;
      (switch3_state == false) ? digitalWrite(gpio_switch3, LOW) : digitalWrite(gpio_switch3, HIGH);
      param->updateAndReport(val);
    }
  }
}

void bt0PopCallback(void *ptr)
{
  uint32_t dual_state = 0; 
  bt0.getValue(&dual_state); // Get the state value of dual state button component
  if(dual_state == 1) 
  {
    digitalWrite(gpio_switch0, HIGH);
  }
  else
  {
    digitalWrite(gpio_switch0, LOW);
  }
}

void bt1PopCallback(void *ptr)
{
  uint32_t dual_state = 0; 
  bt0.getValue(&dual_state); // Get the state value of dual state button component
  if(dual_state == 1) 
  {
    digitalWrite(gpio_switch1, HIGH);
  }
  else
  {
    digitalWrite(gpio_switch1, LOW);
  }
}

void bt2PopCallback(void *ptr)
{
  uint32_t dual_state = 0; 
  bt0.getValue(&dual_state); // Get the state value of dual state button component
  if(dual_state == 1) 
  {
    digitalWrite(gpio_switch2, HIGH);
  }
  else
  {
    digitalWrite(gpio_switch2, LOW);
  }
}

void bt3PopCallback(void *ptr)
{
  uint32_t dual_state = 0; 
  bt0.getValue(&dual_state); // Get the state value of dual state button component
  if(dual_state == 1) 
  {
    digitalWrite(gpio_switch3, HIGH);
  }
  else
  {
    digitalWrite(gpio_switch3, LOW);
  }
}

void sht31d_read_function()
{
  SHT31D result = sht3xd.periodicFetchData();
  temperature = result.t;
  humidity = result.rh;
  // temp_nextion = temp * 100;
  // hum_nextion = hum * 100;
}

void nextion_send_data()
{
  Serial1.print("page2.va2.txt=\"" + String((float)soilPH / 10.0) + "\"\xFF\xFF\xFF"); // va2 is for ph data record
  Serial1.print("page2.va3.txt=\"" + String((float)soilTemperature / 10.0) + "\"\xFF\xFF\xFF"); // va3 is for soil temp data record
  Serial1.print("page2.va4.txt=\"" + String((float)soilHumidity / 10.0) + "\"\xFF\xFF\xFF"); // va4 is for soil moist data record
  Serial1.print("page2.va5.txt=\"" + String(soilConductivity) + "\"\xFF\xFF\xFF"); // va5 is for ec data record
  Serial1.print("page2.va6.txt=\"" + String(temperature) + "\"\xFF\xFF\xFF"); // va6 is for temp data record
  Serial1.print("page2.va7.txt=\"" + String(humidity) + "\"\xFF\xFF\xFF"); // va7 is for hum data record
  Serial1.print("page2.va8.txt=\"" + String(nitrogen) + "\"\xFF\xFF\xFF"); // va8 is for nitrogen data record
  Serial1.print("page2.va9.txt=\"" + String(phosphorus) + "\"\xFF\xFF\xFF"); // va9 is for phosphorus data record
  Serial1.print("page2.va10.txt=\"" + String(potassium) + "\"\xFF\xFF\xFF"); // va10 is for potassium data record
  Serial1.print("page0.x0.val=" + String((int)soilPH) + "\xFF\xFF\xFF");  // x0 is ph xfloat in nextion
  Serial1.print("page0.x1.val=" + String((int)soilTemperature) + "\xFF\xFF\xFF");  // x1 is soil temp xfloat in nextion
  Serial1.print("page0.x2.val=" + String((int)soilHumidity) + "\xFF\xFF\xFF");  // x2 is soil moist xfloat in nextion
  Serial1.print("page0.x3.val=" + String((int)soilConductivity) + "\xFF\xFF\xFF");  // x3 is ec xfloat in nextion
  Serial1.print("page0.x4.val=" + String((int)temperature * 100) + "\xFF\xFF\xFF"); // x4 is temp xfloat in nextion ,,, temp_nextion (tested)
  Serial1.print("page0.x5.val=" + String((int)humidity * 100) + "\xFF\xFF\xFF");  // x5 is hum xfloat in nextion  ,,, hum_nextion (tested)
  Serial1.print("page0.x6.val=" + String((int)nitrogen) + "\xFF\xFF\xFF");  // x6 is nitrogen xfloat in nextion
  Serial1.print("page0.x7.val=" + String((int)phosphorus) + "\xFF\xFF\xFF");  // x7 is phosphorus xfloat in nextion 
  Serial1.print("page0.x8.val=" + String((int)potassium) + "\xFF\xFF\xFF");  // x8 is potassium xfloat in nextion  
}

void npk_read_function()
{
  byte queryData[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
  byte receivedData[19];

  Serial2.write(queryData, sizeof(queryData));  // Send the query data to the NPK sensor
  delay(1000);  // Wait for 1 second

  if (Serial2.available() >= sizeof(receivedData)) {  // Check if there are enough bytes available to read
    Serial2.readBytes(receivedData, sizeof(receivedData));  // Read the received data into the receivedData array

    // Parse and print the received data in decimal format
    soilHumidity = (receivedData[3] << 8) | receivedData[4];
    soilTemperature = (receivedData[5] << 8) | receivedData[6];
    soilConductivity = (receivedData[7] << 8) | receivedData[8];
    soilPH = (receivedData[9] << 8) | receivedData[10];
    nitrogen = (receivedData[11] << 8) | receivedData[12];
    phosphorus = (receivedData[13] << 8) | receivedData[14];
    potassium = (receivedData[15] << 8) | receivedData[16];

    // Serial.print("Soil Humidity: ");
    // Serial.println((float)soilHumidity / 10.0);
    // Serial.print("Soil Temperature: ");
    // Serial.println((float)soilTemperature / 10.0);
    // Serial.print("Soil Conductivity: ");
    // Serial.println(soilConductivity);
    // Serial.print("Soil pH: ");
    // Serial.println((float)soilPH / 10.0);
    // Serial.print("Nitrogen: ");
    // Serial.println(nitrogen);
    // Serial.print("Phosphorus: ");
    // Serial.println(phosphorus);
    // Serial.print("Potassium: ");
    // Serial.println(potassium);
  }
}

void setup()
{
  Serial.begin(115200); // For ESP Serial Debug 
  Serial1.begin(115200); // Communication for HMI
  Serial2.begin(4800); // Communication for RS485 Sensor
	Wire.begin(); // Default SDA & SCL is GPIO 8 and GPIO 9
	sht3xd.begin(0x44); // I2C address: 0x44 or 0x45
	sht3xd.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ);
  pinMode(gpio_reset, INPUT);
  pinMode(gpio_switch0, OUTPUT);
  pinMode(gpio_switch1, OUTPUT);
  pinMode(gpio_switch2, OUTPUT);
  pinMode(gpio_switch3, OUTPUT);
  digitalWrite(gpio_switch0, LOW);
  digitalWrite(gpio_switch1, LOW);
  digitalWrite(gpio_switch2, LOW);
  digitalWrite(gpio_switch3, LOW);
  Node my_node;
  my_node = RMaker.initNode("Agrivoltaics Rainmaker IoT");
  my_temperature = new TemperatureSensor("Temperature (°C)");
  my_humidity = new TemperatureSensor("Humidity (%RH)");
  my_soil_humidity = new TemperatureSensor("Soil Moisture (%RH)");
  my_soil_temperature = new TemperatureSensor("Soil Temperature (°C)");
  my_soil_conductivity = new TemperatureSensor("Soil Conductivity (μS/cm)");
  my_soil_ph = new TemperatureSensor("Soil pH");
  my_soil_nitrogen = new TemperatureSensor("Nitrogen (mg/kg)");
  my_soil_phosphorus = new TemperatureSensor("Phosphorus (mg/kg)");
  my_soil_potassium = new TemperatureSensor("Potassium (mg/kg)");
  my_switch0 = new Switch("Switch Pump 1", &gpio_switch0);
  my_switch1 = new Switch("Switch Pump 2", &gpio_switch1);
  my_switch2 = new Switch("Switch Valve 1", &gpio_switch2);
  my_switch3 = new Switch("Switch Valve 2", &gpio_switch3);
  if (!my_switch0 || !my_switch1 || !my_switch2 || !my_switch3) 
  {
   return;
  }
  my_switch0->addCb(write_callback);
  my_switch1->addCb(write_callback);
  my_switch2->addCb(write_callback);
  my_switch3->addCb(write_callback);
  my_node.addDevice(*my_switch0);
  my_node.addDevice(*my_switch1);
  my_node.addDevice(*my_switch2);
  my_node.addDevice(*my_switch3);
  my_node.addDevice(*my_temperature);
  my_node.addDevice(*my_humidity);
  my_node.addDevice(*my_soil_humidity);
  my_node.addDevice(*my_soil_temperature);
  my_node.addDevice(*my_soil_conductivity);
  my_node.addDevice(*my_soil_ph);
  my_node.addDevice(*my_soil_nitrogen);
  my_node.addDevice(*my_soil_phosphorus);
  my_node.addDevice(*my_soil_potassium);
  RMaker.enableOTA(OTA_USING_TOPICS);
  RMaker.setTimeZone("Asia/Jakarta");
  RMaker.enableTZService();
  RMaker.enableSchedule();
  RMaker.enableScenes();
  initAppInsights();
  RMaker.enableSystemService(SYSTEM_SERV_FLAGS_ALL, 2, 2, 2);
  RMaker.start();
  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
  nexInit(); // problem solved by modificated Nextion library itself from NexHardware.cpp
  bt0.attachPop(bt0PopCallback, &bt0);
  bt1.attachPop(bt1PopCallback, &bt1);
  bt2.attachPop(bt2PopCallback, &bt2);
  bt3.attachPop(bt3PopCallback, &bt3);
  Timer.setInterval(20000);
}

void loop()
{
  if (Timer.isReady() && WiFi.status() == WL_CONNECTED)
  {
    my_temperature->updateAndReportParam("Temperature", temperature);
    my_humidity->updateAndReportParam("Temperature", humidity);
    my_soil_humidity->updateAndReportParam("Temperature", (float)soilHumidity);
    my_soil_temperature->updateAndReportParam("Temperature", (float)soilTemperature);
    my_soil_conductivity->updateAndReportParam("Temperature", (float)soilConductivity);
    my_soil_ph->updateAndReportParam("Temperature", (float)soilPH);
    my_soil_nitrogen->updateAndReportParam("Temperature", (float)nitrogen);
    my_soil_phosphorus->updateAndReportParam("Temperature", (float)phosphorus);
    my_soil_potassium->updateAndReportParam("Temperature", (float)potassium);
    Timer.reset();
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    sht31d_read_function();
    npk_read_function();
    nextion_send_data();
  }
  // Read GPIO0 (external button to reset device
  // if (digitalRead(gpio_reset) == LOW) 
  // {  //Push button pressed
  //   Serial.printf("Reset Button Pressed!\n");
  //   // Key debounce handling
  //   delay(100);
  //   int startTime = millis();
  //   while (digitalRead(gpio_reset) == LOW) 
  //   {
  //     delay(50);
  //   }
  //   int endTime = millis();

  //   if ((endTime - startTime) > 10000) 
  //   {
  //     // If key pressed for more than 10secs, reset all
  //     Serial.printf("Reset to factory.\n");
  //     RMakerFactoryReset(2);
  //   } 
  //   else if ((endTime - startTime) > 3000) 
  //   {
  //     Serial.printf("Reset Wi-Fi.\n");
  //     // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
  //     RMakerWiFiReset(2);
  //   }
  // }
  nexLoop(nex_listen_list);
}