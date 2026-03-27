#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>

void setTFT();
void updateTFT();

#define TFT_CS   7
#define TFT_DC   9
#define TFT_RST  8
#define TFT_BL   6 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define STEP_PIN 10
#define DIR_PIN 12

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

#define DHTPIN 5
#define DHTTYPE DHT22
float airTemp;
float airHum;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  dht.begin();
  // TFT stuff
  setTFT();

  // Stepper stuff
  stepper.setMaxSpeed(3000);
  stepper.setSpeed(3000);
}

void loop() {
  stepper.runSpeed();
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();

  updateTFT();
  delay(2000);
}

void setTFT(){
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(1);
  tft.print("Motor v2");
  tft.setCursor(10, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Temperature: " + String(airTemp, 0) + " C");
  tft.setCursor(10, 50);
  tft.print("Humidity: " + String(airHum, 0) + " %");
  tft.setCursor(10, 70);
  tft.print("Speed:");
  tft.setCursor(10, 90);
  tft.print("Multiplier:");

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 128); // ~50% brightness
}

void updateTFT(){

  // ---- Temperature ----
  tft.setCursor(10, 30);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.print("Temperature: ");

  if (airTemp >= 0 && airTemp <= 20) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else if ((airTemp >= -10 && airTemp < 0) || (airTemp > 20 && airTemp <= 30)) {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }

  tft.print(String(airTemp, 0) + " C   ");


  // ---- Humidity ----
  tft.setCursor(10, 50);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.print("Humidity: ");

  if (airHum <= 60) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else if (airHum <= 70) {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }

  tft.print(String(airHum, 0) + " %   ");
}