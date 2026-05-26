#include <MKRWAN.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define BATTERY_PIN A6

DHT dht(DHTPIN, DHTTYPE);
LoRaModem modem;

#include "arduino_secrets.h"

String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

void setup() {

  Serial.begin(115200);
  dht.begin();

  while (!Serial);

  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1);
  }

  modem.joinOTAA(appEui, appKey);
  modem.minPollInterval(120);
}

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int sensorValue = analogRead(BATTERY_PIN);
  float batteryVoltage = sensorValue * (3.3 / 1023.0) * 2.0;

  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Battery: ");
  Serial.println(batteryVoltage);

  int temp = (int)temperature;
  int hum  = (int)humidity;
  int batt = (int)(batteryVoltage * 100);

  byte payload[3];
  payload[0] = temp;
  payload[1] = hum;
  payload[2] = batt;

  modem.beginPacket();
  modem.write(payload, 3);
  modem.endPacket(true);

  delay(50000);
}
