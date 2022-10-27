#include <SPI.h>
#include <WiFiNINA.h>
#include <MQTT.h>
#include <DHT.h>
#include <Wire.h>  // I2C Library
#include <utility/wifi_drv.h>
#include <Adafruit_GFX.h> // Adafruit OLED Display
#include <Adafruit_SSD1306.h> // Adafruit OLED Display

#define DHTTYPE DHT11
#define DHTPIN 2

#define GREEN 25
#define RED 26
#define BLUE 27

#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


const char ssid[] = "SibirienAP";
const char pass[] = "Siberia51244";
unsigned long lastMillis = 0;

WiFiClient net;
MQTTClient client;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFiDrv::analogWrite(RED, 255);  //
    WiFiDrv::analogWrite(GREEN, 0);
    WiFiDrv::analogWrite(BLUE, 0);
    delay(250);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("MBgwJDQlBy0SHiAlFDY4JBk", "MBgwJDQlBy0SHiAlFDY4JBk", "cNERyk5wAiYNbzI/qA82Luu7")) {
    Serial.print(".");
    delay(1000);
  }
  WiFiDrv::analogWrite(RED, 0);  //
  WiFiDrv::analogWrite(GREEN, 255);
  WiFiDrv::analogWrite(BLUE, 0);
  Serial.println("\nconnected!");

  //client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (payload == "on") {
    digitalWrite(LED_BUILTIN, 1);
  } else if (payload == "off") {
    digitalWrite(LED_BUILTIN, 0);
  }

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);

  WiFiDrv::pinMode(25, OUTPUT);  //define green pin
  WiFiDrv::pinMode(26, OUTPUT);  //define red pin
  WiFiDrv::pinMode(27, OUTPUT);  //define blue pin

  pinMode(LED_BUILTIN, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
	{ // Address for 128x64
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}

  dht.begin();
  WiFi.begin(ssid, pass);

  client.begin("mqtt3.thingspeak.com", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 30000) {
    lastMillis = millis();
    String query = "field1= ";
    query.concat(dht.readTemperature());
    query.concat("&field2=");
    query.concat(dht.readHumidity());
    Serial.println(query);
    display.clearDisplay();
    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,0); // Start at top-left corner
    display.println(query);
    display.display();
    client.publish("channels/1910486/publish", query);
  }
}