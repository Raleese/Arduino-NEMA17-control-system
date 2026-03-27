# NEMA17 System (Arduino Nano + PlatformIO)

Control a NEMA17 stepper motor with live speed adjustment from buttons, while displaying motor/environment data on an ST7735 TFT screen.

This project uses:
- Arduino Nano (ATmega328P)
- TMC2209 driver
- NEMA17 stepper motor with a 51:1 planetary gearbox
- DHT22 temperature/humidity sensor
- ST7735S SPI TFT display
- 3 momentary buttons for speed control

## Features

- Continuous stepper rotation using AccelStepper (`runSpeed`)
- Speed increase/decrease from hardware buttons
- Selectable speed multiplier (1x, 2..10, then 100x, then back to 1x)
- TFT UI showing:
	- Temperature (color-coded)
	- Humidity (color-coded)
	- Current speed
	- Current multiplier
- Adjustable TFT backlight brightness (currently ~50%)

## Hardware Pin Map

The current firmware in [src/main.cpp](src/main.cpp) uses this mapping:

### TFT ST7735

- CS: D7
- DC: D9
- RST: D8
- BL (backlight PWM): D6
- SPI lines: Arduino Nano hardware SPI (MOSI/SCK)

### Stepper Driver

- STEP: D10
- DIR: D12

### DHT22

- Data: D5

### Buttons (INPUT_PULLUP)

- Plus: D4
- Change multiplier: D3
- Minus: D2

Button logic is active-low (pressed = LOW).

## Software Stack

- Framework: Arduino
- Build system: PlatformIO
- Board environment: `nanoatmega328`
- Libraries:
	- Adafruit ST7735 and ST7789 Library
	- Adafruit GFX Library (dependency)
	- AccelStepper
	- DHT sensor library

Configured in [platformio.ini](platformio.ini).

## Project Structure

```
NEMA17_system/
├─ platformio.ini
├─ src/
│  └─ main.cpp
├─ include/
├─ lib/
└─ test/
```

## Build and Upload

### Prerequisites

- VS Code
- PlatformIO extension
- USB cable connected to Arduino Nano

### Commands

From this project folder:

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

## Runtime Behavior

- Motor runs continuously at current `Speed`.
- Press plus/minus to change speed by `speedMultiplier`.
- Press change button to cycle multiplier values.
- DHT22 values are sampled during button interactions and then refreshed on TFT.

## Tuning

You can quickly tune these values in [src/main.cpp](src/main.cpp):

- `Speed` (default startup speed)
- `stepper.setMaxSpeed(7000)`
- TFT backlight level: `analogWrite(TFT_BL, 128)`
- Debounce delay: `delay(200)`