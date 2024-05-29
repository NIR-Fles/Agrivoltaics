#include "ClosedCube_SHT31D.h"
#include <Wire.h>
#include "Nextion.h" // must change the library itself called NexUpload.cpp from SoftwareSerial.h to HardwareSerial.h due to esp32 compatibility [fixed]
#include <RMaker.h>
#include <WiFi.h> // error occured not found bits\error_constants.h -> must copy from other related file [fixed]
#include <WiFiProv.h>
#include <AppInsights.h> // error occured when try to compile gen_insight_packages.py -> gtw bjir tiba tiba bisa gitu [fixed]

ClosedCube_SHT31D sht3xd;

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
static uint8_t gpio_0 = 0;
static uint8_t gpio_switch0 = RGB_BUILTIN;
static uint8_t gpio_switch1 = RGB_BUILTIN;
static uint8_t gpio_switch2 = RGB_BUILTIN;
static uint8_t gpio_switch3 = RGB_BUILTIN;
static Switch *my_switch0 = NULL;
static Switch *my_switch1 = NULL;
static Switch *my_switch2 = NULL;
static Switch *my_switch3 = NULL;
bool switch0_state = false;
bool switch1_state = false;
bool switch2_state = false;
bool switch3_state = false;

unsigned long previousMillis = 0;
const long interval = 1000; // Interval in milliseconds

float temperature, humidity;
int temp_nextion, hum_nextion;

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

void sht31dReadingValue()
{
  SHT31D result = sht3xd.periodicFetchData();
  temperature = result.t;
  humidity = result.rh;
  temp_nextion = temperature * 100;
  hum_nextion = humidity * 100;
}

void nextionSendData()
{
  Serial1.print("page2.va6.txt=\"" + String(temperature) + "\"\xFF\xFF\xFF"); // va6 i for temp data record
  Serial1.print("page2.va7.txt=\"" + String(humidity) + "\"\xFF\xFF\xFF"); // va7 is for hum data record
  Serial1.print("page0.x4.val=" + String(temp_nextion) + "\xFF\xFF\xFF"); // x4 is temp xfloat in nextion
  Serial1.print("page0.x5.val=" + String(hum_nextion) + "\xFF\xFF\xFF");  // x5 is hum xfloat in nextion 
}

void setup()
{
  Serial.begin(115200); // For ESP Serial Debug 
  Serial1.begin(115200); // Communication for HMI
	Wire.begin(); // Default SDA & SCL is GPIO 8 and GPIO 9
	sht3xd.begin(0x44); // I2C address: 0x44 or 0x45
	sht3xd.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ);
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
  RMaker.enableOTA(OTA_USING_TOPICS);
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
}

void loop()
{
  if (digitalRead(gpio_0) == LOW)  // Push button pressed
  { 
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_0) == LOW) 
    {
      delay(50);
    }
    int endTime = millis();

    if ((endTime - startTime) > 10000) 
    {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      RMakerFactoryReset(2);
    } 
    else if ((endTime - startTime) > 3000) 
    {
      Serial.printf("Reset Wi-Fi.\n");
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    } 
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    sht31dReadingValue();
    nextionSendData();
  }
  nexLoop(nex_listen_list);
}