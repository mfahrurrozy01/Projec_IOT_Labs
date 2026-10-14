#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_IR     14
#define PIN_RED    12
#define PIN_YELLOW 13
#define PIN_GREEN  16

#define IR_DETECTED LOW

const unsigned long READ_INTERVAL = 50;
const unsigned long LCD_INTERVAL  = 1000;
const unsigned long STABLE_TIME   = 1000;

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool occupied = false;
bool candidate = false;
unsigned long candidateSince = 0;
unsigned long occupiedSince = 0;
unsigned long lastRead = 0;
unsigned long lastLcd = 0;

bool readIr() {
  return digitalRead(PIN_IR) == IR_DETECTED;
}

void setLeds(bool r, bool y, bool g) {
  digitalWrite(PIN_RED, r);
  digitalWrite(PIN_YELLOW, y);
  digitalWrite(PIN_GREEN, g);
}

void updateLeds() {
  if (candidate != occupied) {
    setLeds(LOW, HIGH, LOW);
  } else if (occupied) {
    setLeds(HIGH, LOW, LOW);
  } else {
    setLeds(LOW, LOW, HIGH);
  }
}

void updateLcd() {
  lcd.setCursor(0, 0);
  lcd.print(occupied ? "Slot A1: TERISI " : "Slot A1: KOSONG ");

  lcd.setCursor(0, 1);
  if (occupied) {
    unsigned long sec = (millis() - occupiedSince) / 1000;
    char buf[17];
    snprintf(buf, sizeof(buf), "Durasi %02lu:%02lu    ", (sec / 60) % 100, sec % 60);
    lcd.print(buf);
  } else {
    lcd.print("Silakan parkir  ");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_IR, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   ParkSense    ");
  lcd.setCursor(0, 1);
  lcd.print(" Memulai...     ");

  setLeds(HIGH, LOW, LOW); delay(300);
  setLeds(LOW, HIGH, LOW); delay(300);
  setLeds(LOW, LOW, HIGH); delay(300);

  occupied = readIr();
  candidate = occupied;
  occupiedSince = millis();
  updateLeds();
  updateLcd();
  Serial.println("ParkSense siap.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;
    bool raw = readIr();

    if (raw != candidate) {
      candidate = raw;
      candidateSince = now;
    }

    if (candidate != occupied && now - candidateSince >= STABLE_TIME) {
      occupied = candidate;
      if (occupied) {
        occupiedSince = now;
      }
      Serial.println(occupied ? "Slot terisi." : "Slot kosong.");
      updateLcd();
    }

    updateLeds();
  }

  if (now - lastLcd >= LCD_INTERVAL) {
    lastLcd = now;
    updateLcd();
  }
}
