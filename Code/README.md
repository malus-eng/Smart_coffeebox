# Smart Coffee Box Firmware

This folder contains the Arduino firmware that runs the Smart Coffee Box system.  
The program monitors temperature, humidity, lid status, and volatile organic compounds (TVOCs),
and automatically manages air circulation and alerts to maintain coffee storage quality.

---

## Features

- Reads:
  - **Temperature & Humidity** using DHT22
  - **TVOC** using SGP30
  - **Lid open/closed** via limit switch
- Drives:
  - **Cooling/air circulation fan** via MOSFET
  - **Piezo buzzer** for lid left-open warning
- Displays live status on a **0.96" SSD1306 OLED**
- Includes:
  - Hysteresis logic for humidity + temperature
  - Duty-cycle fan control to preserve aroma
  - Automatic lid detection lockout (fan disabled when open)
  - Delayed buzzer alarm after 60+ seconds lid-open
  - Dryness warning blink if humidity is too low

---

## Required Libraries

Install these through **Arduino Library Manager**:

| Library | Used For |
|--------|-----------|
| Adafruit SGP30 | TVOC readings |
| DHT sensor library | Humidity and temperature |
| Adafruit SSD1306 | OLED control |
| Adafruit GFX | OLED graphics |
| Wire (built-in) | I2C communications |

Search by name in:  
**Sketch → Include Library → Manage Libraries…**

---

## Hardware Compatibility

Designed for:
- Arduino **MKR WAN 1310** (3.3 V logic)

Compatible with:
- Arduino Nano / Uno (if pin mapping updated to 5 V logic)
- ESP32 (with minor pin edits)

---

## Pin Mapping (Summary)

| Signal | MCU Pin |
|--------|---------|
| DHT22 DATA | D2 |
| Limit Switch | D3 |
| Fan MOSFET Gate | D5 |
| Buzzer | D6 |
| SGP30 SDA / OLED SDA | 11 (I2C) |
| SGP30 SCL / OLED SCL | 12 (I2C) |

Full pin map available in `/hardware/pinmap.md`

---

## How It Works — Core Logic

### 1. Sensor Reading
- DHT22 → Temperature + Humidity
- SGP30 → TVOC (Indoor Air Quality)
- Limit switch → Detects lid state

### 2. Trigger Decisions
- Humidity + temperature thresholds include hysteresis
- If lid is **closed** and readings exceed limits → fan requested
- If lid is **open** → fan always disabled

### 3. Fan Control with Duty Cycle
- When triggered:
  - **ON 60 sec**
  - **OFF 180 sec**
- Extends moisture retention & aroma

### 4. Lid Warning System
- If lid remains open for **>60 sec**:
  - Buzzer beeps ON/OFF (200/800 ms pattern)
- Resets instantly when lid closes

### 5. Display Output
- Temperature / Humidity
- TVOC
- Fan status
- Freshness suggestion (based on TVOC)
- Dryness warning (if <35% RH)

---

## Adjustable Parameters
Edit these constants to tune behavior:

```cpp
HUM_ON_THRESHOLD
HUM_OFF_THRESHOLD
HUM_TOO_DRY_THRESHOLD
TEMP_ON_THRESHOLD
TEMP_OFF_THRESHOLD
FAN_ON_TIME_MS
FAN_OFF_TIME_MS
LID_OPEN_WARNING_DELAY_MS
BUZZER_ON_MS
BUZZER_OFF_MS
