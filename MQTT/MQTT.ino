#include <SPI.h>
#include <WiFiNINA.h>
#include <MQTT.h>
#include <DHT.h>
#include <Wire.h> // I2C Library


#define DHTTYPE DHT11
#define DHTPIN 2


const char ssid[] = "SibirienAP";
const char pass[] = "Siberia51244";
unsigned long lastMillis = 0;

WiFiClient net;
MQTTClient client;
DHT dht(DHTPIN, DHTTYPE);

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "mangosting", "CXzamZsmmy1iSedX")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  //client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (payload == "on")
  {
    digitalWrite(LED_BUILTIN, 1);
  }
  else if (payload == "off") {
    digitalWrite(LED_BUILTIN, 0);
  }

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, HIGH);
  dht.begin();
  WiFi.begin(ssid, pass);

  client.begin("mangosting.cloud.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("DHT11", String(dht.readTemperature()));
  }
}