#include "HomeSpan.h"
#include <DHT.h>

// Define DHT22 pin and type
#define DHTPIN 18
#define DHTTYPE DHT22

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Forward declarations of service structures
struct DEV_TemperatureSensor;
struct DEV_HumiditySensor;

// Global pointers to access services
DEV_TemperatureSensor *globalTempSensor;
DEV_HumiditySensor *globalHumSensor;

// Variables to manage sensor reading timing
unsigned long lastReadTime = 0;

// Declare updateSensors() so it can be called in the structs
void updateSensors();

// Define the TemperatureSensor struct
struct DEV_TemperatureSensor : Service::TemperatureSensor {
  SpanCharacteristic *temp;
  
  DEV_TemperatureSensor() : Service::TemperatureSensor() {
    temp = new Characteristic::CurrentTemperature();
  }
  
  void loop() {
    updateSensors(); // Calls the declared function
  }
};

// Define the HumiditySensor struct
struct DEV_HumiditySensor : Service::HumiditySensor {
  SpanCharacteristic *hum;
  
  DEV_HumiditySensor() : Service::HumiditySensor() {
    hum = new Characteristic::CurrentRelativeHumidity();
  }
  
  void loop() {
    // No action needed; updates handled by temperature sensor's loop
  }
};

// Define updateSensors() after the structs are fully defined
void updateSensors() {
  if (millis() - lastReadTime > 2000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t)) {
      globalTempSensor->temp->setVal(t); // Now temp is known
    }
    if (!isnan(h)) {
      globalHumSensor->hum->setVal(h);   // Now hum is known
    }
    lastReadTime = millis();
  }
}

// Setup and loop functions
void setup() {
  Serial.begin(115200);
  dht.begin();
  homeSpan.begin();
  
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify("Weather Station");
      new Characteristic::Name("Weather Station");
    globalTempSensor = new DEV_TemperatureSensor();
    globalHumSensor = new DEV_HumiditySensor();
}

void loop() {
  homeSpan.poll();
}