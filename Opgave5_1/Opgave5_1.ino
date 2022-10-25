#include <SPI.h> // SPI Library
#include <Wire.h> // I2C Library
#include <Adafruit_GFX.h> // Adafruit OLED Display
#include <Adafruit_SSD1306.h> // Adafruit OLED Display
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DHTTYPE DHT11     // DHT 22 (AM2302)
#define DHTPIN 2 

// Objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

uint32_t delayMS;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() 
{
	Serial.begin(9600);
  dht.begin();

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
	{ // Address for 128x64
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}
}

void loop() 
{
	display.clearDisplay();
	display.setTextSize(2); // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(0,0); // Start at top-left corner
  display.print("Temperature: ");
  display.print(dht.readTemperature());
	display.setTextSize(1); // Normal 1:1 pixel scale
  display.write(247);
	display.setTextSize(2); // Normal 1:1 pixel scale

  display.println();
  display.print("Humidity: ");
  display.println(dht.readHumidity());

 

  // DateTime now = rtc.now();
	// display.println(daysOfTheWeek[now.dayOfTheWeek()]);
	// display.print(now.hour());
	// display.print(':');
	// display.print(now.minute());
  // display.print(':');
	// display.print(now.second());

	display.display();
}