# Pin Mapping

## Microcontroller

- Board: Arduino MKR WAN 1310  
- Logic voltage: 3.3 V  
- I2C bus (MKR WAN 1310): SDA = pin 11, SCL = pin 12  
- Note: The schematic is drawn with an Arduino Nano as a logical equivalent MCU.  
  The functional mapping to the MKR WAN 1310 is as listed below.

## MCU Connection Table

| Component         | Pin / Signal                     | MCU Pin / Rail | Notes                                      |
| ----------------- | -------------------------------- | -------------- | ------------------------------------------ |
| **DHT22**         | VCC                              | 3.3 V          | Power                                      |
| DHT22             | GND                              | GND            | Ground (common)                            |
| DHT22             | DATA                             | D2             | Temperature & humidity data                |
| **Limit Switch**  | One side                         | GND            | Connected to ground                        |
| Limit Switch      | Other side (signal)              | D3             | Digital input, configured as INPUT_PULLUP  |
| **Fan MOSFET**    | Gate (via 220 Ω resistor)        | D5             | PWM / on–off control for DC fan            |
| **Buzzer**        | Positive (via 100 Ω resistor)    | D6             | Digital output for beeps / alerts          |
| Buzzer            | Negative                         | GND            | Return path                                |
| **SGP30**         | VCC                              | 3.3 V          | Power                                      |
| SGP30             | GND                              | GND            | Ground (common)                            |
| SGP30             | SDA                              | 11 (SDA)       | I2C data line                              |
| SGP30             | SCL                              | 12 (SCL)       | I2C clock line                             |
| **OLED SSD1306**  | VCC                              | 3.3 V          | Power                                      |
| OLED SSD1306      | GND                              | GND            | Ground (common)                            |
| OLED SSD1306      | SDA                              | 11 (SDA)       | I2C data line (shared with SGP30)          |
| OLED SSD1306      | SCL                              | 12 (SCL)       | I2C clock line (shared with SGP30)         |
| **Power rails**   | 3.3 V rail                      | —              | Feeds MCU, DHT22, SGP30, OLED              |
| Power rails       | 5 V rail                         | —              | Feeds DC fan positive terminal             |
| Power rails       | GND rail                         | —              | All grounds connected together             |

## Discrete Components and High-Current Path

These connections are **between components**, not directly to MCU pins, but are important for reproducing the circuit:

- **MOSFET (IRLZ44N, viewed from front with pins down)**  
  - Left pin  → Gate (from MCU D5 through ~220 Ω resistor)  
  - Middle pin → Drain (to Fan negative terminal)  
  - Right pin → Source (to GND)

- **Fan (40×40×10 mm, 5 V DC brushless)**  
  - Fan +  → 5 V supply  
  - Fan −  → MOSFET Drain  

- **Flyback diode (1N4001)**  
  - Cathode (banded side) → Fan + (5 V side)  
  - Anode                → Fan − / MOSFET Drain  

- **Gate resistor (~220 Ω)**  
  - One side → MCU D5  
  - Other side → MOSFET Gate  

- **Buzzer series resistor (~100 Ω)**  
  - One side → MCU D6  
  - Other side → Buzzer positive terminal  

- **Grounding**  
  - MCU GND, DHT22 GND, SGP30 GND, OLED GND, MOSFET Source,  
    Buzzer negative terminal, and Fan negative (through MOSFET)  
    are all tied to the same common GND rail.
