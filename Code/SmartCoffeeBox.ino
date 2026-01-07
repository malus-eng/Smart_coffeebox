#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------------- Pin configuration ----------------------
const int FAN_PIN           = 5;   // MOSFET gate (through resistor)
const int LIMIT_SWITCH_PIN  = 3;   // Lid switch: one side to GND, other to this pin
const int DHT_PIN           = 2;   // DHT22 data pin
const int BUZZER_PIN        = 6;   // PKM22EPP-40 buzzer (+ via 100R)

// ---------------------- Sensor types ---------------------------
#define DHTTYPE DHT22

// ---------------------- OLED configuration ---------------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // No dedicated reset pin

// ---------------------- Thresholds -----------------------------
const float HUM_ON_THRESHOLD      = 65.0f;  // RH%: turn humidity trigger ON
const float HUM_OFF_THRESHOLD     = 60.0f;  // RH%: turn humidity trigger OFF (hysteresis)
const float HUM_TOO_DRY_THRESHOLD = 35.0f;  // RH%: show dryness warning

const float TEMP_ON_THRESHOLD     = 28.0f;  // deg C: turn temperature trigger ON
const float TEMP_OFF_THRESHOLD    = 26.0f;  // deg C: turn temperature trigger OFF

// Fan duty cycle to preserve aroma
const unsigned long FAN_ON_TIME_MS  = 60000UL;   // 60 seconds ON
const unsigned long FAN_OFF_TIME_MS = 180000UL;  // 180 seconds OFF

// Lid open warning timing
const unsigned long LID_OPEN_WARNING_DELAY_MS = 60000UL; // Start beeping after 60 seconds
const unsigned long BUZZER_ON_MS             = 200UL;    // 200 ms beep ON
const unsigned long BUZZER_OFF_MS            = 800UL;    // 800 ms beep OFF

// Timing configuration
const unsigned long SENSOR_INTERVAL_MS  = 2000UL;  // Sensor read interval
const unsigned long DISPLAY_INTERVAL_MS = 500UL;   // Display update interval
const unsigned long BLINK_INTERVAL_MS   = 500UL;   // Dryness warning blink interval

// ---------------------- Global objects -------------------------
DHT dht(DHT_PIN, DHTTYPE);
Adafruit_SGP30 sgp;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------------- Global state variables -----------------
float gTemperature = NAN;
float gHumidity    = NAN;
uint16_t gTVOC     = 0;

bool dhtOK = false;
bool sgpOK = false;

bool gLidClosed = false;

// Triggers with hysteresis
bool humidityTriggerActive = false;
bool tempTriggerActive     = false;

// Fan control state
bool fanDemand     = false; // Environment wants fan activity
bool lastFanDemand = false;
bool fanIsOn       = false;
unsigned long fanStateChangeTime = 0;

// Timing helpers
unsigned long lastSensorTime  = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastBlinkTime   = 0;

// Dryness warning blink state
bool drynessWarningVisible = false;

// Lid open warning / buzzer state
bool previousLidClosed      = true;
unsigned long lidOpenStartTime     = 0;
bool lidWarningActive       = false;
bool buzzerOn               = false;
unsigned long buzzerStateChangeTime = 0;

// ---------------------- Helper: compute freshness text ---------
const char* getFreshnessText(uint16_t tvoc, bool valid) {
  if (!valid) {
    return "Freshness: unknown";
  }

  // Simple absolute thresholds in ppb.
  // You can tune these later based on real data.
  if (tvoc >= 250) {
    return "Freshness: HIGH - use now";
  } else if (tvoc >= 80) {
    return "Freshness: MEDIUM";
  } else {
    return "Freshness: LOW - not ideal";
  }
}

// ---------------------- Helper: read sensors -------------------
void readSensors() {
  // Lid switch: HIGH means lid is closed (based on your wiring)
  gLidClosed = (digitalRead(LIMIT_SWITCH_PIN) == HIGH);

  // DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  if (isnan(h) || isnan(t)) {
    dhtOK = false;
  } else {
    dhtOK = true;
    gHumidity = h;
    gTemperature = t;
  }

  // SGP30
  if (sgp.IAQmeasure()) {
    sgpOK = true;
    gTVOC = sgp.TVOC;
  } else {
    sgpOK = false;
  }

  // Serial debug (no CO2 printed)
  Serial.println("----- Sensor Reading -----");
  if (dhtOK) {
    Serial.print("Temperature: ");
    Serial.print(gTemperature);
    Serial.println(" C");

    Serial.print("Humidity   : ");
    Serial.print(gHumidity);
    Serial.println(" %");
  } else {
    Serial.println("DHT22 error");
  }

  if (sgpOK) {
    Serial.print("TVOC: ");
    Serial.print(gTVOC);
    Serial.println(" ppb");
  } else {
    Serial.println("SGP30 error");
  }

  Serial.print("Lid closed: ");
  Serial.println(gLidClosed ? "YES" : "NO");
  Serial.println("--------------------------");
  Serial.println();
}

// ---------------------- Helper: update triggers ----------------
void updateTriggers() {
  // Humidity trigger with hysteresis
  if (!dhtOK) {
    humidityTriggerActive = false;
  } else {
    if (gHumidity >= HUM_ON_THRESHOLD) {
      humidityTriggerActive = true;
    } else if (gHumidity <= HUM_OFF_THRESHOLD) {
      humidityTriggerActive = false;
    }
  }

  // Temperature trigger with hysteresis
  if (!dhtOK) {
    tempTriggerActive = false;
  } else {
    if (gTemperature >= TEMP_ON_THRESHOLD) {
      tempTriggerActive = true;
    } else if (gTemperature <= TEMP_OFF_THRESHOLD) {
      tempTriggerActive = false;
    }
  }

  // Fan demand: environment wants some air circulation
  fanDemand = (humidityTriggerActive || tempTriggerActive);

  // Safety: do not run fan when lid is open
  if (!gLidClosed) {
    fanDemand = false;
  }
}

// ---------------------- Helper: update fan state ---------------
void updateFanState(unsigned long now) {
  if (!fanDemand) {
    // No demand: force fan OFF
    fanIsOn = false;
    digitalWrite(FAN_PIN, LOW);
    lastFanDemand = false;
    return;
  }

  if (!lastFanDemand) {
    // Demand just turned from false to true: start a new duty cycle
    fanIsOn = true;
    fanStateChangeTime = now;
    digitalWrite(FAN_PIN, HIGH);
    lastFanDemand = true;
    return;
  }

  // Demand is still true: run duty cycle (ON for FAN_ON_TIME_MS, OFF for FAN_OFF_TIME_MS)
  if (fanIsOn) {
    if (now - fanStateChangeTime >= FAN_ON_TIME_MS) {
      fanIsOn = false;
      fanStateChangeTime = now;
      digitalWrite(FAN_PIN, LOW);
    }
  } else {
    if (now - fanStateChangeTime >= FAN_OFF_TIME_MS) {
      fanIsOn = true;
      fanStateChangeTime = now;
      digitalWrite(FAN_PIN, HIGH);
    }
  }
}

// ---------------------- Helper: update lid warning & buzzer ----
void updateLidWarningAndBuzzer(unsigned long now) {
  // When lid is closed, reset timer and stop buzzer
  if (gLidClosed) {
    lidWarningActive = false;
    lidOpenStartTime = now;

    if (buzzerOn) {
      noTone(BUZZER_PIN);
      buzzerOn = false;
    }

    previousLidClosed = true;
    return;
  }

  // Lid is open
  if (previousLidClosed) {
    // Lid has just been opened
    lidOpenStartTime = now;
    lidWarningActive = false;

    if (buzzerOn) {
      noTone(BUZZER_PIN);
      buzzerOn = false;
    }
  }

  // Check if lid has been open long enough to trigger warning
  if (!lidWarningActive && (now - lidOpenStartTime >= LID_OPEN_WARNING_DELAY_MS)) {
    lidWarningActive = true;
    buzzerOn = false;
    buzzerStateChangeTime = now;
  }

  // If warning is active, run a simple beep pattern (ON/OFF)
  if (lidWarningActive) {
    if (buzzerOn) {
      // Currently ON: turn it OFF after BUZZER_ON_MS
      if (now - buzzerStateChangeTime >= BUZZER_ON_MS) {
        noTone(BUZZER_PIN);
        buzzerOn = false;
        buzzerStateChangeTime = now;
      }
    } else {
      // Currently OFF: turn it ON after BUZZER_OFF_MS
      if (now - buzzerStateChangeTime >= BUZZER_OFF_MS) {
        tone(BUZZER_PIN, 4000); // Around 4 kHz for PKM22EPP-40
        buzzerOn = true;
        buzzerStateChangeTime = now;
      }
    }
  }

  previousLidClosed = false;
}

// ---------------------- Helper: update OLED display ------------
void updateDisplay(unsigned long now) {
  (void)now; // unused for now, kept for future use

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("Smart Coffee Box");

  // Temperature and humidity line
  if (dhtOK) {
    display.print("T: ");
    display.print(gTemperature, 1);
    display.print("C  H: ");
    display.print(gHumidity, 1);
    display.println("%");
  } else {
    display.println("DHT22: error");
  }

  // TVOC line only
  if (sgpOK) {
    display.print("TVOC: ");
    display.print(gTVOC);
    display.println(" ppb");
  } else {
    display.println("TVOC: error");
  }

  // Fan status
  display.print("Fan: ");
  display.println(fanIsOn ? "ON" : "OFF");

  // Freshness indicator
  const char* freshness = getFreshnessText(gTVOC, sgpOK);
  display.println(freshness);

  // Bottom area: dryness warning (blinking)
  if (dhtOK && gHumidity < HUM_TOO_DRY_THRESHOLD) {
    if (drynessWarningVisible) {
      display.setCursor(0, 48); // lower half of the screen
      display.println("Too dry! Use less desiccant!");
    }
  }

  display.display();
}

// ---------------------- Arduino setup -------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial on some boards
  }

  Serial.println();
  Serial.println("=== Smart Coffee Box ===");

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); // One side of switch to GND

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);

  dht.begin();

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1) {
      delay(1000);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Coffee Box");
  display.println("Initializing...");
  display.display();

  if (!sgp.begin()) {
    Serial.println("SGP30 init failed");
    display.println("SGP30 error");
    display.display();
    while (1) {
      delay(1000);
    }
  }

  if (!sgp.IAQinit()) {
    Serial.println("SGP30 IAQinit failed");
  } else {
    Serial.println("SGP30 initialized");
  }

  unsigned long now = millis();
  lastSensorTime       = now;
  lastDisplayTime      = now;
  lastBlinkTime        = now;
  fanStateChangeTime   = now;
  lidOpenStartTime     = now;
  buzzerStateChangeTime = now;
}

// ---------------------- Arduino loop --------------------------
void loop() {
  unsigned long now = millis();

  // Periodic sensor readings
  if (now - lastSensorTime >= SENSOR_INTERVAL_MS) {
    lastSensorTime = now;
    readSensors();
    updateTriggers();
  }

  // Fan control based on triggers and duty cycle
  updateFanState(now);

  // Lid warning and buzzer control
  updateLidWarningAndBuzzer(now);

  // Blink state for dryness warning
  if (now - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = now;
    drynessWarningVisible = !drynessWarningVisible;
  }

  // Periodic OLED update
  if (now - lastDisplayTime >= DISPLAY_INTERVAL_MS) {
    lastDisplayTime = now;
    updateDisplay(now);
  }
}
