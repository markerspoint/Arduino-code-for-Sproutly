#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // RX, TX (HC-05)

const int moisturePin = A0;

// Motor pins
const int IN1 = 7;
const int IN2 = 6;

// LEDs
const int ledPin1 = 13;
const int ledPin2 = 12;

// Calibration
const int DRY_VALUE = 850;
const int WET_VALUE = 780;

// Sensitivity tuning
const int DRY_THRESHOLD = 25;
const int WET_THRESHOLD = 40;

// Timing
unsigned long lastSendTime = 0;
unsigned long pumpStartTime = 0;
unsigned long lastWaterTime = 0;

const unsigned long MAX_PUMP_TIME = 5000; // 5 seconds
unsigned long waterCooldown = 30000;      // default 30 seconds, app can change this

// States
bool pumpRunning = false;
bool manualMode = false; // NEW: true when user manually turned ON pump

int moisture = 0;
int dryness = 0;

// Pump control
void pumpOn() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(ledPin1, HIGH);
}

void pumpOff() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(ledPin1, LOW);
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  Serial.begin(9600);
  BT.begin(9600);

  pumpOff();

  Serial.println("System Ready - APP CONTROLLED COOLDOWN");
}

void loop() {

  // Bluetooth control
  if (BT.available()) {
    char cmd = BT.read();

    // Manual pump ON
    if (cmd == '1') {
      manualMode = true;          // NEW
      pumpRunning = true;
      pumpStartTime = millis();
      pumpOn();
      Serial.println("Manual ON");
    }

    // Manual pump OFF
    else if (cmd == '0') {
      manualMode = false;         // NEW
      pumpRunning = false;
      lastWaterTime = millis();
      pumpOff();
      Serial.println("Manual OFF");
    }

    // Cooldown command from app
    // Example:
    // C30  = 30 seconds
    // C180 = 3 minutes
    else if (cmd == 'C') {
      int seconds = BT.parseInt();

      if (seconds >= 5 && seconds <= 3600) {
        waterCooldown = seconds * 1000UL;

        BT.print("Cooldown set to ");
        BT.print(seconds);
        BT.println(" seconds");

        Serial.print("Cooldown set to ");
        Serial.print(seconds);
        Serial.println(" seconds");
      } else {
        BT.println("Invalid cooldown. Use 5 to 3600 seconds.");
        Serial.println("Invalid cooldown value received.");
      }
    }
  }

  // Sensor read
  int rawValue = analogRead(moisturePin);

  moisture = map(rawValue, DRY_VALUE, WET_VALUE + 20, 0, 100);
  moisture = constrain(moisture, 0, 100);

  moisture += 15;
  moisture = constrain(moisture, 0, 100);

  dryness = 100 - moisture;
  dryness = dryness * 0.4;
  dryness = constrain(dryness, 0, 100);

  // Auto watering logic (ONLY when not in manual mode)
  if (!manualMode &&
      !pumpRunning &&
      moisture < DRY_THRESHOLD &&
      millis() - lastWaterTime >= waterCooldown) {

    pumpRunning = true;
    pumpStartTime = millis();
    pumpOn();
  }

  // Auto wet-stop (ONLY when not in manual mode)
  if (!manualMode &&
      pumpRunning &&
      moisture >= WET_THRESHOLD) {

    pumpRunning = false;
    lastWaterTime = millis();
    pumpOff();
  }

  // Safety stop after 5 seconds (applies to both auto/manual)
  if (pumpRunning && millis() - pumpStartTime >= MAX_PUMP_TIME) {
    pumpRunning = false;
    pumpOff();

    // return to auto behavior after manual timeout
    if (manualMode) {
      manualMode = false;
      Serial.println("Manual safety timeout -> back to AUTO");
    }
  }

  // Send data every 300ms
  if (millis() - lastSendTime >= 300) {
    lastSendTime = millis();

    BT.print("M:");
    BT.print(moisture);
    BT.print("% ");

    BT.print("D:");
    BT.print(dryness);
    BT.print("% ");

    BT.print("C:");
    BT.print(waterCooldown / 1000);
    BT.print("s ");

    BT.print("MODE:");
    BT.println(manualMode ? "MANUAL" : "AUTO");

    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print(" | Moisture: ");
    Serial.print(moisture);
    Serial.print("% | Dryness: ");
    Serial.print(dryness);
    Serial.print("% | Cooldown: ");

    if (millis() - lastWaterTime < waterCooldown) {
      Serial.print("ACTIVE ");
      Serial.print((waterCooldown - (millis() - lastWaterTime)) / 1000);
      Serial.print("s left");
    } else {
      Serial.print("READY");
    }

    Serial.print(" | Mode: ");
    Serial.println(manualMode ? "MANUAL" : "AUTO");
  }
}
