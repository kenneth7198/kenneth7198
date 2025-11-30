// SR04 + MIC candle effect on LED
// Interaction:
// - When distance < 100cm, blow into MIC (A0) to trigger candle-like fade
// - Candle effect: start very bright, then decay to a dim flickering state
// Pins per existing sketches:
//   HC-SR04: trig D12, echo D11
//   MIC: A0
//   LED: D6 (PWM)

// Ultrasonic pins
const int trigPin = 12;
const int echoPin = 11;

// MIC and LED
const int micPin = A0;
const int ledPin = 6;  // PWM capable

// Behavior tunables
const long distanceThresholdCm = 100;  // only react when closer than this
const int micBlowThreshold = 600;      // adjust to your MIC module (raw ADC)
const int candleBright = 255;          // initial brightness
const int candleDim = 40;              // target dim level
const unsigned long candleDurationMs = 2000; // total decay time

// Flicker parameters
const int flickerRange = 20;           // +/- around target dim
const unsigned long flickerIntervalMs = 60;  // update flicker rate

// State
bool candleActive = false;
unsigned long candleStartMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);
}

long readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout ~30ms
  if (duration == 0) return 9999; // out of range
  long distance = (duration / 2) / 29.4;
  return distance;
}

void startCandle() {
  candleActive = true;
  candleStartMs = millis();
  analogWrite(ledPin, candleBright);
}

void updateCandle() {
  if (!candleActive) return;
  unsigned long elapsed = millis() - candleStartMs;
  if (elapsed >= candleDurationMs) {
    // Hold at dim with flicker
    int base = candleDim;
    int flick = random(-flickerRange, flickerRange + 1);
    int level = base + flick;
    if (level < 0) level = 0;
    if (level > 255) level = 255;
    analogWrite(ledPin, level);
  } else {
    // Linear decay from bright to dim over candleDurationMs
    float p = (float)elapsed / (float)candleDurationMs; // 0..1
    int level = (int)((1.0 - p) * candleBright + p * candleDim);
    analogWrite(ledPin, level);
  }
}

void loop() {
  long distance = readDistanceCm();
  int micRaw = analogRead(micPin);

  // Debug
  Serial.print("Dist:"); Serial.print(distance); Serial.print("cm  MIC:"); Serial.println(micRaw);

  if (distance < distanceThresholdCm) {
    // Only when close enough, blowing triggers candle
    if (micRaw > micBlowThreshold) {
      startCandle();
    }
  } else {
    // Out of range: turn off candle effect
    candleActive = false;
    analogWrite(ledPin, 0);
  }

  updateCandle();
  delay(30);
}
