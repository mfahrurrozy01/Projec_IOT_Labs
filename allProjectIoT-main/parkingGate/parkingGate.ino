#include <Servo.h>

#define PIN_TRIG  5
#define PIN_ECHO  4
#define PIN_SERVO 14

const int ANGLE_CLOSED = 0;
const int ANGLE_OPEN   = 180;

const float DETECT_CM = 15.0;
const int DETECT_COUNT = 3;

const unsigned long READ_INTERVAL = 100;
const unsigned long STEP_INTERVAL = 15;
const unsigned long CLOSE_DELAY   = 3000;

Servo gate;

bool gateOpen = false;
int currentAngle = ANGLE_CLOSED;
int targetAngle = ANGLE_CLOSED;
int hits = 0;
unsigned long lastRead = 0;
unsigned long lastStep = 0;
unsigned long lastSeen = 0;

float readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duration == 0) {
    return 999.0;
  }
  return duration * 0.0343 / 2.0;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  gate.attach(PIN_SERVO);
  gate.write(ANGLE_CLOSED);
  Serial.println("GateMate siap.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;
    float dist = readDistanceCm();

    if (dist < DETECT_CM) {
      hits++;
    } else {
      hits = 0;
    }

    bool vehicle = hits >= DETECT_COUNT;

    if (vehicle) {
      lastSeen = now;
      if (!gateOpen) {
        gateOpen = true;
        targetAngle = ANGLE_OPEN;
        Serial.println("Kendaraan terdeteksi, palang dibuka.");
      }
    } else if (gateOpen && now - lastSeen >= CLOSE_DELAY) {
      gateOpen = false;
      targetAngle = ANGLE_CLOSED;
      Serial.println("Area kosong, palang ditutup.");
    }

    Serial.printf("Jarak: %.1f cm | Palang: %s\n", dist, gateOpen ? "TERBUKA" : "TERTUTUP");
  }

  if (currentAngle != targetAngle && now - lastStep >= STEP_INTERVAL) {
    lastStep = now;
    currentAngle += (targetAngle > currentAngle) ? 1 : -1;
    gate.write(currentAngle);
  }
}
