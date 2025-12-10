# Smart Coffee Box 

An Arduino-based smart storage box for coffee beans and tea leaves.  
It monitors **temperature**, **humidity**, and **TVOC**, and automatically controls a fan, OLED display and buzzer to protect flavour during storage.

> Hardware prototype built with an Arduino MKR WAN 1310, DHT22, SGP30, limit switch, 40×40 mm fan, 0.96" OLED and a piezo buzzer inside a 3D-printed PLA enclosure.

---

## Features

- **Environmental monitoring**
  - DHT22 for **temperature** and **relative humidity** inside the box
  - SGP30 for **TVOC** (Total Volatile Organic Compounds)
  - Limit switch to detect whether the **lid is closed**

- **Automatic fan control**
  - Turns on when humidity or temperature are too high
  - Uses **hysteresis** to avoid rapid on/off flicker
  - Runs in **duty cycles** (e.g. 60 s ON / 180 s OFF) to balance drying and aroma preservation
  - Only runs when the lid is closed

- **User feedback**
  - 0.96" I²C OLED displays:
    - Temperature (°C) and humidity (%RH)
    - TVOC (ppb)
    - Fan status (ON/OFF)
    - A simple **3-level freshness indicator** based on TVOC
    - A blinking warning message if the box is **too dry**  

- **Lid open warning**
  - If the lid is left open for more than **60 seconds**, a piezo buzzer starts a periodic beep pattern to remind the user to close the box.

---

## Hardware

**Microcontroller**

- Arduino **MKR WAN 1310**

**Sensors**

- **DHT22** – temperature and humidity
- **SGP30** – TVOC (total volatile organic compounds)
- **Limit switch** – lid open / closed detection

**Actuators & UI**

- **40×40 mm DC fan** (driven by IRLZ44N MOSFET + 1N4001 flyback diode)
- **0.96" I²C OLED display** (SSD1306, 128×64)
- **PKM22EPP-40 piezo buzzer** (driven directly from an IO pin through ~100 Ω)

**Enclosure**

- 3D-printed **PLA** box
- Dedicated **desiccant compartment** on/near the lid
- Cut-out for the fan and air flow path across the desiccant
- Mounting points for sensors and the limit switch

More details about wiring and mechanical design are in:

- `hardware/wiring.md`
- `hardware/enclosure-notes.md` (optional)

---

## Wiring (summary)

**MKR WAN 1310 → Modules**

- **DHT22**
  - VCC → 3V3
  - GND → GND
  - DATA → D2

- **Limit switch**
  - One side → GND
  - Other side → D3 (configured as `INPUT_PULLUP`)

- **SGP30 (I²C)**
  - VCC → 3V3
  - GND → GND
  - SDA → A4 (SDA)
  - SCL → A5 (SCL)

- **0.96" OLED (SSD1306, I²C)**
  - VCC → 3V3
  - GND → GND
  - SDA → A4 (SDA)
  - SCL → A5 (SCL)

- **Fan + MOSFET**
  - Fan `+` → 5V
  - Fan `−` → MOSFET **Drain** (IRLZ44N middle pin)
  - MOSFET **Source** → GND
  - MOSFET **Gate** → D5 (through ~220 Ω resistor)
  - **1N4001** diode:
    - Diode `−` (band side) → Fan `+` (5V)
    - Diode `+` → Fan `−` / MOSFET Drain

- **Buzzer (PKM22EPP-40 piezo)**
  - Buzzer `+` → D6 (through ~100 Ω resistor)
  - Buzzer `−` → GND

> All grounds must be common (MKR GND, fan GND, sensors, buzzer).

---

## Firmware

The main Arduino sketch lives in:

```text
firmware/SmartCoffeeBox/SmartCoffeeBox.ino
