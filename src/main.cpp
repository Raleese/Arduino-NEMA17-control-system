#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>

void setTFT();
void updateTFT();
void handleButtons();
void drawBatteryBar(float batteryVoltage);

#define VOLTAGE_PIN A0 

#define TFT_CS   7
#define TFT_DC   9
#define TFT_RST  8
#define TFT_BL   6

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define STEP_PIN 10
#define DIR_PIN 12
float speedMultiplier = 0.1;
float Speed = 10;

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

#define DHTPIN 5
#define DHTTYPE DHT22
float airTemp;
float airHum;

DHT dht(DHTPIN, DHTTYPE);

const int plusPin = 4;
const int changePin = 3;
const int minusPin = 2;

unsigned long lastUpdateTime = 0;
unsigned long lastVoltageUpdate = 0;
const unsigned long voltageUpdateInterval = 2000; // ms
const float batteryMaxVoltage = 12.7;
const float batteryMinVoltage = 11.0;

const float changeSize = 0.1;
const float lastVoltage = 0.0;

bool changeWasPressed = false;
unsigned long changePressStart = 0;
const unsigned long longPressTime = 800; // ms

void setup() {
  // Button stuff
  pinMode(plusPin, INPUT_PULLUP);
  pinMode(changePin, INPUT_PULLUP);
  pinMode(minusPin, INPUT_PULLUP);

  // DHT stuff

  dht.begin();
  // TFT stuff
  setTFT();

  // Stepper stuff
  stepper.setMaxSpeed(7000);
  stepper.setSpeed(Speed);
}

void loop() {
  stepper.runSpeed();
  handleButtons();
}

void setTFT(){
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);

  // ---- Title ----
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setTextSize(1);
  tft.print("Motor v2");

  tft.drawLine(10, 20, 120, 20, ST77XX_WHITE);
  tft.setTextColor(ST77XX_WHITE);


  // ---- Speed ----
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.print("Speed");

  tft.setCursor(80, 30);
  tft.print(String(Speed));


  // ---- Multiplier ----
  tft.setTextSize(1);
  tft.setCursor(10, 60);
  tft.print("Increment");

  tft.setCursor(80, 60);
  tft.print( "+" +String(speedMultiplier));

  // ---- Line ----

  tft.drawLine(10, 140, 120, 140, ST77XX_WHITE);

  // ---- Temperature ----
  tft.setTextSize(1);
  tft.setCursor(10, 145);
  tft.print(String(airTemp, 0) + "C");


  // ---- Humidity ----
  tft.setTextSize(1);
  tft.setCursor(100, 145);
  tft.print(String(airHum, 0) + "%");

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 128); // ~50% brightness
}

void updateTFT(){

  // ---- Temperature value ----
  if (airTemp >= 0 && airTemp <= 20) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else if ((airTemp >= -10 && airTemp < 0) || (airTemp > 20 && airTemp <= 30)) {
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }

  tft.setTextSize(1);
  tft.setCursor(10, 145);
  tft.print(String(airTemp, 0) + "C  ");

  // ---- Humidity value ----
  if (airHum <= 60) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else if (airHum <= 70) {
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }

  tft.setTextSize(1);
  tft.setCursor(100, 145);
  tft.print(String(airHum, 0) + "%  ");

  // ---- Speed value ----
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(80, 30);
  tft.print(String(Speed) + "  ");

  // ---- Multiplier value ----
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(80, 60);
  tft.print( "+" + String(speedMultiplier) + "  ");
}

void drawBatteryBar(float batteryVoltage) {
  const int barX = 10;
  const int barY = 130;
  const int barWidth = 100;
  const int barHeight = 8;

  float batteryPercent = (batteryVoltage - batteryMinVoltage) / (batteryMaxVoltage - batteryMinVoltage);
  if (batteryPercent < 0.0) {
    batteryPercent = 0.0;
  } else if (batteryPercent > 1.0) {
    batteryPercent = 1.0;
  }

  int fillWidth = (int)((barWidth - 2) * batteryPercent);

  tft.drawRect(barX, barY, barWidth, barHeight, ST77XX_WHITE);
  tft.fillRect(barX + 1, barY + 1, barWidth - 2, barHeight - 2, ST77XX_BLACK);

  uint16_t barColor = ST77XX_GREEN;
  if (batteryPercent < 0.25) {
    barColor = ST77XX_RED;
  } else if (batteryPercent < 0.5) {
    barColor = ST77XX_YELLOW;
  }

  if (fillWidth > 0) {
    tft.fillRect(barX + 1, barY + 1, fillWidth, barHeight - 2, barColor);
  }
}

void handleButtons() {
  if (digitalRead(plusPin) == LOW) {
    Speed += speedMultiplier;

    airTemp = dht.readTemperature();
    airHum = dht.readHumidity();

    updateTFT();
    delay(200); // Debounce delay
  }
  if (digitalRead(minusPin) == LOW) {
    Speed -= speedMultiplier;

    airTemp = dht.readTemperature();
    airHum = dht.readHumidity();

    updateTFT();
    delay(200); // Debounce delay

  }


  bool changePressed = (digitalRead(changePin) == LOW);

  if (changePressed && !changeWasPressed) {
    changeWasPressed = true;
    changePressStart = millis();
  }

  if (!changePressed && changeWasPressed) {
    changeWasPressed = false;
    unsigned long pressDuration = millis() - changePressStart;

    if (pressDuration >= longPressTime) {
      airTemp = dht.readTemperature();
      airHum = dht.readHumidity();
    } else {
      if (speedMultiplier < 10) {
        speedMultiplier += 1;
      } else if (speedMultiplier >= 10 && speedMultiplier < 100) {
        speedMultiplier = 100;
      } else {
        speedMultiplier = 0.1;
      }
      
    }
    updateTFT();
    delay(200); // Debounce delay
  }
  stepper.setSpeed(Speed);
}
