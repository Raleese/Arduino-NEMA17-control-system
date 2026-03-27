#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>

void setTFT();
void updateTFT();
void handleButtons();

#define TFT_CS   7
#define TFT_DC   9
#define TFT_RST  8
#define TFT_BL   6 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define STEP_PIN 10
#define DIR_PIN 12
int speedMultiplier = 1;
int Speed = 1000;

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

// ---- Title ----
tft.setCursor(10, 10);
tft.setTextColor(ST77XX_YELLOW);
tft.setTextSize(1);
tft.print("Motor v2");

tft.drawLine(10, 20, 120, 20, ST77XX_WHITE);


// ---- Temperature ----
tft.setTextSize(1);
tft.setTextColor(ST77XX_WHITE);
tft.setCursor(10, 30);
tft.print("Temperature");

tft.setTextSize(2);
tft.setCursor(80, 40);
tft.print(String(airTemp, 0) + "C");


// ---- Humidity ----
tft.setTextSize(1);
tft.setCursor(10, 60);
tft.print("Humidity");

tft.setTextSize(2);
tft.setCursor(80, 70);
tft.print(String(airHum, 0) + "%");


// ---- Speed ----
tft.setTextSize(1);
tft.setCursor(10, 90);
tft.print("Speed");

tft.setTextSize(2);
tft.setCursor(80, 100);
tft.print(String(Speed));


// ---- Multiplier ----
tft.setTextSize(1);
tft.setCursor(10, 120);
tft.print("Multiplier");

tft.setTextSize(2);
tft.setCursor(80, 130);
tft.print(String(speedMultiplier) + "x");

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 128); // ~50% brightness
}

void updateTFT(){

  // ---- Temperature label ----
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(10, 30);
  tft.print("Temperature   ");

  // ---- Temperature value ----
  if (airTemp >= 0 && airTemp <= 20) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else if ((airTemp >= -10 && airTemp < 0) || (airTemp > 20 && airTemp <= 30)) {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }

  tft.setTextSize(2);
  tft.setCursor(80, 40);
  tft.print(String(airTemp, 0) + "C  ");


  // ---- Humidity label ----
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(10, 60);
  tft.print("Humidity   ");

  // ---- Humidity value ----
  if (airHum <= 60) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else if (airHum <= 70) {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }

  tft.setTextSize(2);
  tft.setCursor(80, 70);
  tft.print(String(airHum, 0) + "%  ");


  // ---- Speed label ----
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(10, 90);
  tft.print("Speed   ");

  // ---- Speed value ----
  tft.setTextSize(2);
  tft.setCursor(80, 100);
  tft.print(String(Speed) + "   ");


  // ---- Multiplier label ----
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(10, 120);
  tft.print("Multiplier   ");

  // ---- Multiplier value ----
  tft.setTextSize(2);
  tft.setCursor(80, 130);
  tft.print(String(speedMultiplier) + "x  ");
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
  if (digitalRead(changePin) == LOW) {
    airTemp = dht.readTemperature();
    airHum = dht.readHumidity();

    if (speedMultiplier < 10) {
      speedMultiplier += 1;
    } else if (speedMultiplier >= 10 && speedMultiplier < 100) {
      speedMultiplier = 100;
    } else {
      speedMultiplier = 1;
    }
    updateTFT();
    delay(200); // Debounce delay
  }
  stepper.setSpeed(Speed);
}
