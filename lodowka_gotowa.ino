#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

unsigned long delayTime;

float humidity;
float temperature;

char humidityMessage[16];
char temperatureMessage[16];

int relayTemperaturePin = D3;
int relayHumidityPin = D4;

const float SET_TEMPERATURE = 12.0;
const float SET_HUMIDITY = 80.0;

LiquidCrystal_I2C lcd(0x3F, 16, 2);
void setup() {
  Serial.begin(9600);
  
  pinMode(relayTemperaturePin, OUTPUT);
  digitalWrite(relayTemperaturePin, HIGH);
  pinMode(relayHumidityPin, OUTPUT);
  digitalWrite(relayHumidityPin, HIGH);

  bme.begin(0x76);  
  lcd.init();
  lcd.backlight();
}
void loop() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();

  printOnLcd(temperature, humidity);

  handleTemperatureRelay(temperature);
  handleHumidityRelay(humidity);  
  delay(40000);
}

void printOnLcd(float temperature, float humidity) {
  sprintf(temperatureMessage, "Temp: %.1f", temperature);
  sprintf(humidityMessage, "Wilgoc: %.1f", humidity);  
  lcd.setCursor(0, 0);
  lcd.print(temperatureMessage); // Print the string "Hello World!"
  lcd.setCursor(0, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
  lcd.print(humidityMessage);
}

void handleTemperatureRelay(float temperature) {
  int relayState;
  if (temperature > SET_TEMPERATURE) {
    relayState = LOW; // ON
  } else {
    relayState = HIGH; // OFF
  }
  digitalWrite(relayTemperaturePin, relayState);
}

void handleHumidityRelay(float humidity) {
  int relayState;
  if (humidity < SET_HUMIDITY) {
    relayState = LOW;
  } else {
    relayState = HIGH;
  }
  digitalWrite(relayHumidityPin, relayState);
}
