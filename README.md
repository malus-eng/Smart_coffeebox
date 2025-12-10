# Smart Coffee Box 

An Arduino-based smart storage box for coffee beans and tea leaves.  
It monitors temperature, humidity and TVOC, and automatically controls a fan, OLED display and buzzer to protect flavour during storage.

> Hardware prototype built with an Arduino MKR WAN 1310, DHT22, SGP30, limit switch, 40×40 mm fan, 0.96" OLED and a piezo buzzer inside a 3D-printed PLA enclosure.

---

## Features

- **Environmental monitoring**
  - DHT22 for temperature and relative humidity inside the box  
  - SGP30 for TVOC, the total volatile organic compounds in the air  
  - Limit switch to detect whether the lid is closed  

- **Automatic fan control**
  - Turns on when humidity or temperature are too high  
  - Uses hysteresis so the fan does not flicker on and off all the time  
  - Runs in duty cycles, for example 60 seconds on and 180 seconds off, to balance drying and aroma preservation  
  - Only runs when the lid is closed  

- **User feedback**
  - 0.96" I²C OLED shows  
    - Temperature in °C and humidity in %RH  
    - TVOC in ppb  
    - Fan status, on or off  
    - A simple three-level freshness indicator based on TVOC  
    - A blinking warning message when the box is too dry  

- **Lid-open warning**
  - If the lid is left open for more than 60 seconds, a piezo buzzer starts a periodic beep pattern to remind the user to close the box  

---

## Hardware

**Microcontroller**

- Arduino MKR WAN 1310  

**Sensors**

- DHT22 for temperature and humidity  
- SGP30 for TVOC  
- Limit switch for lid open or closed detection  

**Actuators and user interface**

- 40×40 mm DC fan driven by an IRLZ44N MOSFET and a 1N4001 flyback diode  
- 0.96" I²C OLED display based on an SSD1306 controller, 128×64 pixels  
- PKM22EPP-40 piezo buzzer driven from an IO pin through a series resistor of about 100 Ω  

**Enclosure**

- 3D-printed PLA box  
- Dedicated desiccant compartment on or near the lid  
- Cut-out for the fan and an air flow path across the desiccant  
- Mounting points for sensors and the limit switch  

More details about wiring and mechanical design can live in:

- `hardware/wiring.md`  
- `hardware/enclosure-notes.md`  

---

## Wiring – summary

**MKR WAN 1310 to modules**

- **DHT22**  
  - VCC → 3V3  
  - GND → GND  
  - DATA → D2  

- **Limit switch**  
  - One side → GND  
  - Other side → D3, configured as `INPUT_PULLUP`  

- **SGP30 over I²C**  
  - VCC → 3V3  
  - GND → GND  
  - SDA → A4, SDA line  
  - SCL → A5, SCL line  

- **0.96" OLED, SSD1306 over I²C**  
  - VCC → 3V3  
  - GND → GND  
  - SDA → A4, shared with SGP30  
  - SCL → A5, shared with SGP30  

- **Fan with MOSFET driver**  
  - Fan positive → 5V  
  - Fan negative → MOSFET drain, the middle pin of the IRLZ44N  
  - MOSFET source → GND  
  - MOSFET gate → D5 through a resistor of about 220 Ω  
  - 1N4001 diode for flyback  
    - Diode band side → fan positive, 5V  
    - Other side → fan negative and MOSFET drain  

- **Buzzer, PKM22EPP-40**  
  - Buzzer positive → D6 through a resistor of about 100 Ω  
  - Buzzer negative → GND  

All grounds must be common, including MKR ground, fan ground, sensors and buzzer.

---

## Firmware

The main Arduino sketch lives in:

```text
firmware/SmartCoffeeBox/SmartCoffeeBox.ino

