// Combined MIC (A0) + DHT11 (D2) LED level driver on D6, D5, D4, D3
// Behavior:
// - Higher sound and higher temperature -> more LEDs on (up to 4)
// - Lower sound and lower temperature -> fewer LEDs on (down to 1)
// Wiring:
//   DHT11 data: D2
//   MIC analog: A0
//   LEDs: D6, D5, D4, D3 (active HIGH)

#include <dht11.h>
dht11 DHT11;

// Pins
const byte PIN_DHT = 2;     // DHT11 data pin
const byte PIN_MIC = A0;    // MIC analog input
const byte LED_PINS[4] = {6, 5, 4, 3};

// State
float humidity = 0.0;
float temperature = 0.0;

// Tunables
// Expected temperature range for mapping (°C)
const float TEMP_MIN = 15.0;   // cold
const float TEMP_MAX = 35.0;   // hot
// Sound thresholds for normalization (raw ADC)
const int MIC_LOW = 200;       // quiet
const int MIC_HIGH = 800;      // loud

// Helper: constrain and map float to 0..1
float fmap01(float x, float inMin, float inMax) {
  if (inMax == inMin) return 0.0;
  if (x < inMin) x = inMin;
  if (x > inMax) x = inMax;
  return (x - inMin) / (inMax - inMin);
}

// Helper: set N LEDs on from highest pin to lowest (D6..D3)
void setLedLevel(uint8_t level) {
  // level: 1..4 (ensure min 1)
  if (level < 1) level = 1;
  if (level > 4) level = 4;
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(LED_PINS[i], (i < level) ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(9600);
  // LED pins
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
}

void loop() {
  // Read DHT11
  int dhtStatus = DHT11.read(PIN_DHT);
  if (dhtStatus == 0) {
    humidity = DHT11.humidity;
    temperature = DHT11.temperature;
  } else {
    // If read failed, keep previous values (or optionally set defaults)
  }

  // Read MIC
  int micRaw = analogRead(PIN_MIC);

  // Normalize inputs to 0..1
  float micNorm = fmap01((float)micRaw, MIC_LOW, MIC_HIGH);
  float tempNorm = fmap01(temperature, TEMP_MIN, TEMP_MAX);

  // Combine: weighted average; adjust weights as desired
  const float wSound = 0.6;   // sound weight
  const float wTemp = 0.4;    // temperature weight
  float combined = (micNorm * wSound) + (tempNorm * wTemp);

  // Map combined 0..1 to LED level 1..4
  // Use thresholds to avoid flicker
  uint8_t level;
  if (combined >= 0.75) {
    level = 4;
  } else if (combined >= 0.5) {
    level = 3;
  } else if (combined >= 0.25) {
    level = 2;
  } else {
    level = 1;
  }

  setLedLevel(level);

  // Debug output
  Serial.print("MIC raw:"); Serial.print(micRaw);
  Serial.print("  micNorm:"); Serial.print(micNorm, 2);
  Serial.print("  Temp:"); Serial.print(temperature, 1);
  Serial.print("C  tempNorm:"); Serial.print(tempNorm, 2);
  Serial.print("  Level:"); Serial.println(level);

  delay(100);
}
