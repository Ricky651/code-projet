#include <MKRWAN.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define BATTERY_PIN A1

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
    while (1) {}
  }

  Serial.println(modem.version());
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);

  if (!connected) {
    Serial.println("Connection failed");
    while (1) {}
  }

  modem.minPollInterval(120);
}

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int sensorValue = analogRead(BATTERY_PIN);

  float batteryVoltage = sensorValue * (3.3 / 1023.0);
  batteryVoltage = batteryVoltage * 2;

  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Battery: ");
  Serial.println(batteryVoltage);

  byte payload[3];

  payload[0] = (int)temperature;
  payload[1] = (int)humidity;
  payload[2] = (int)batteryVoltage;

  modem.beginPacket();
  modem.write(payload, 3);
  modem.endPacket(true);

  delay(120000);
}
