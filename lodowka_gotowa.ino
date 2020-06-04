#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <SPI.h>
#include <SHT3x.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
SHT3x sensor;

float humidity;
float temperature;

char humidityMessage[16];
char temperatureMessage[16];

int relayTemperaturePin = D3;
int relayHumidityPin = D4;

const float SET_TEMPERATURE = 12.0;
const float SET_HUMIDITY = 80.0;
const float TEMPERATURE_HYSTERESIS = 1.5;
const float HUMIDITY_HYSTERESIS = 2.5;

#define LOOP_PERIOD_MS  1000 // 1 sec
#define INTERVAL  1 // 2 mins

uint32_t startMillis = 0;
uint32_t currentMillis = 0;
uint16_t loopCnt = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial) continue;

    pinMode(relayTemperaturePin, OUTPUT);
    digitalWrite(relayTemperaturePin, HIGH);
    pinMode(relayHumidityPin, OUTPUT);
    digitalWrite(relayHumidityPin, HIGH);

    sensor.Begin();
    lcd.init();
    lcd.backlight();

    startMillis = millis();
}

void loop() {
    currentMillis = millis();
    temperature = sensor.GetTemperature();
    humidity = sensor.GetRelHumidity();
    sensor.UpdateData();
    if (currentMillis - startMillis >= LOOP_PERIOD_MS) {
        startMillis = currentMillis;

        if (!(loopCnt % (INTERVAL * 60))) {
            handleTemperatureRelay(temperature);
            handleHumidityRelay(humidity);
        }
        loopCnt++;
    }

    printOnLcd(temperature, humidity);
}

void printOnLcd(float temperature, float humidity) {
    String tempRelayState = getRelayState(relayTemperaturePin);
    String humidityRelayState = getRelayState(relayHumidityPin);

    sprintf(temperatureMessage, "Temp:  %.1f %s", temperature, const_cast<char *>(tempRelayState.c_str()));
    sprintf(humidityMessage, "Wilgoc:%.1f %s", humidity, const_cast<char *>(humidityRelayState.c_str()));
    lcd.setCursor(0, 0);
    lcd.print(temperatureMessage); // Print the string "Hello World!"
    lcd.setCursor(0, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
    lcd.print(humidityMessage);
}

void handleTemperatureRelay(float temperature) {
    if (temperature - TEMPERATURE_HYSTERESIS / 2 >= SET_TEMPERATURE) {
        digitalWrite(relayTemperaturePin, LOW);
    } else if (temperature + TEMPERATURE_HYSTERESIS / 2 <= SET_TEMPERATURE) {
        digitalWrite(relayTemperaturePin, HIGH);
    }
}

void handleHumidityRelay(float humidity) {
    if (humidity - HUMIDITY_HYSTERESIS / 2 <= SET_HUMIDITY) {
        digitalWrite(relayHumidityPin, LOW);
    } else if (humidity + HUMIDITY_HYSTERESIS / 2 >= SET_HUMIDITY) {
        digitalWrite(relayHumidityPin, HIGH);
    }
}

String getRelayState(int relayPin) {
    boolean relayState = digitalRead(relayPin);
    String relayMessage;

    if (relayState) {
        relayMessage = "OFF";
    } else {
        relayMessage = "ON ";
    }
    return relayMessage;
}
