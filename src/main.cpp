#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define TFT_CS   7
#define TFT_DC   9
#define TFT_RST  8
#define TFT_BL   6 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define STEP_PIN 10
#define DIR_PIN 12

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
    // TFT stuff
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Hello!");

  pinMode(TFT_BL, OUTPUT);

  analogWrite(TFT_BL, 128); // ~50% brightness

  // Stepper stuff
  stepper.setMaxSpeed(3000);
  stepper.setSpeed(3000);
}

void loop() {
  stepper.runSpeed();
}