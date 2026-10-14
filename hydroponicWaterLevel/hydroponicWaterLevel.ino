#define PIN_LEVEL      A0
#define PIN_SENSOR_PWR 16
#define PIN_RELAY      5
#define PIN_RED        14
#define PIN_YELLOW     12
#define PIN_GREEN      13
#define PIN_BUZZER     4

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

const int LEVEL_EMPTY = 0;
const int LEVEL_FULL  = 550;

const int THRESHOLD_ON  = 30;
const int THRESHOLD_OFF = 80;

const unsigned long READ_INTERVAL = 200;
const unsigned long PUMP_MAX_TIME = 60000;

bool pumpOn = false;
bool fault = false;
unsigned long pumpStart = 0;
unsigned long lastRead = 0;
unsigned long lastBeep = 0;

int levelRaw = 0;

int readLevelPercent() {
  digitalWrite(PIN_SENSOR_PWR, HIGH);
  delay(50);
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(PIN_LEVEL);
    delay(5);
  }
  digitalWrite(PIN_SENSOR_PWR, LOW);
  int raw = sum / 10;
  levelRaw = raw;
  int pct = map(raw, LEVEL_EMPTY, LEVEL_FULL, 0, 100);
  return constrain(pct, 0, 100);
}

void setLeds(bool r, bool y, bool g) {
  digitalWrite(PIN_RED, r);
  digitalWrite(PIN_YELLOW, y);
  digitalWrite(PIN_GREEN, g);
}

void beep(int times, int ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(ms);
    digitalWrite(PIN_BUZZER, LOW);
    delay(ms);
  }
}

void pumpControl(bool on) {
  pumpOn = on;
  digitalWrite(PIN_RELAY, on ? RELAY_ON : RELAY_OFF);
  if (on) {
    pumpStart = millis();
    beep(2, 100);
  } else {
    beep(1, 300);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SENSOR_PWR, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_SENSOR_PWR, LOW);
  digitalWrite(PIN_RELAY, RELAY_OFF);
  digitalWrite(PIN_BUZZER, LOW);
  setLeds(LOW, LOW, LOW);

  setLeds(HIGH, LOW, LOW); delay(300);
  setLeds(LOW, HIGH, LOW); delay(300);
  setLeds(LOW, LOW, HIGH); delay(300);
  Serial.println("HydroWatch siap.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;
    int level = readLevelPercent();

    Serial.printf("Raw: %d | Level air: %d%% | Pompa: %s | Fault: %s\n",
                  levelRaw, level, pumpOn ? "ON" : "OFF", fault ? "YA" : "TIDAK");

    if (fault) {
      if (level >= THRESHOLD_OFF) {
        fault = false;
      }
    } else if (!pumpOn) {
      if (level < THRESHOLD_ON) {
        pumpControl(true);
      }
    } else {
      if (level >= THRESHOLD_OFF) {
        pumpControl(false);
      } else if (now - pumpStart >= PUMP_MAX_TIME) {
        pumpControl(false);
        fault = true;
      }
    }

    if (fault)                       setLeds(HIGH, HIGH, LOW);
    else if (level < THRESHOLD_ON)   setLeds(HIGH, LOW, LOW);
    else if (level < THRESHOLD_OFF)  setLeds(LOW, HIGH, LOW);
    else                             setLeds(LOW, LOW, HIGH);
  }

  if (fault && now - lastBeep >= 3000) {
    lastBeep = now;
    beep(3, 150);
  }
}

//#define PIN_LEVEL      A0
//#define PIN_SENSOR_PWR 16
//#define PIN_RELAY      5
//#define PIN_RED        14
//#define PIN_YELLOW     12
//#define PIN_GREEN      13
//#define PIN_BUZZER     4
//
//#define RELAY_ON  LOW
//#define RELAY_OFF HIGH
//
//const int LEVEL_EMPTY = 0;
//const int LEVEL_FULL  = 550;
//
//const int THRESHOLD_ON  = 30;
//const int THRESHOLD_OFF = 80;
//
//const unsigned long READ_INTERVAL = 1000;
//const unsigned long PUMP_MAX_TIME = 60000;
//const unsigned long PUMP_ON_DELAY = 2000;
//unsigned long lowStart = 0;
//bool lowDetected = false;
//
//bool pumpOn = false;
//bool fault = false;
//unsigned long pumpStart = 0;
//unsigned long lastRead = 0;
//unsigned long lastBeep = 0;
//
//int readLevelPercent() {
//  digitalWrite(PIN_SENSOR_PWR, HIGH);
//  delay(50);
//  long sum = 0;
//  for (int i = 0; i < 10; i++) {
//    sum += analogRead(PIN_LEVEL);
//    delay(5);
//  }
//  digitalWrite(PIN_SENSOR_PWR, LOW);
//  int raw = sum / 10;
//  int pct = map(raw, LEVEL_EMPTY, LEVEL_FULL, 0, 100);
//  return constrain(pct, 0, 100);
//}
//
//void setLeds(bool r, bool y, bool g) {
//  digitalWrite(PIN_RED, r);
//  digitalWrite(PIN_YELLOW, y);
//  digitalWrite(PIN_GREEN, g);
//}
//
//void beep(int times, int ms) {
//  for (int i = 0; i < times; i++) {
//    digitalWrite(PIN_BUZZER, HIGH);
//    delay(ms);
//    digitalWrite(PIN_BUZZER, LOW);
//    delay(ms);
//  }
//}
//
//void pumpControl(bool on) {
//  pumpOn = on;
//  digitalWrite(PIN_RELAY, on ? RELAY_ON : RELAY_OFF);
//  if (on) {
//    pumpStart = millis();
//    beep(2, 100);
//  } else {
//    beep(1, 300);
//  }
//}
//
//void setup() {
//  Serial.begin(115200);
//  pinMode(PIN_SENSOR_PWR, OUTPUT);
//  pinMode(PIN_RELAY, OUTPUT);
//  pinMode(PIN_RED, OUTPUT);
//  pinMode(PIN_YELLOW, OUTPUT);
//  pinMode(PIN_GREEN, OUTPUT);
//  pinMode(PIN_BUZZER, OUTPUT);
//
//  digitalWrite(PIN_SENSOR_PWR, LOW);
//  digitalWrite(PIN_RELAY, RELAY_OFF);
//  digitalWrite(PIN_BUZZER, LOW);
//  setLeds(LOW, LOW, LOW);
//
//  setLeds(HIGH, LOW, LOW); delay(300);
//  setLeds(LOW, HIGH, LOW); delay(300);
//  setLeds(LOW, LOW, HIGH); delay(300);
//  Serial.println("HydroWatch siap.");
//}
//
//void loop() {
//  unsigned long now = millis();
//
//  if (now - lastRead >= READ_INTERVAL) {
//    lastRead = now;
//    int level = readLevelPercent();
//    Serial.printf("Level air: %d%% | Pompa: %s | Fault: %s\n",
//                  level, pumpOn ? "ON" : "OFF", fault ? "YA" : "TIDAK");
//
//    if (fault) {
//      if (level >= THRESHOLD_OFF) {
//        fault = false;
//      }
//    } else if (!pumpOn) {
//      if (level < THRESHOLD_ON) {
//        if (!lowDetected) {
//          lowDetected = true;
//          lowStart = now;
//        } else if (now - lowStart >= PUMP_ON_DELAY) {
//          lowDetected = false;
//          pumpControl(true);
//        }
//      } else {
//        lowDetected = false;
//      }
//    }
//
//    if (fault)                      setLeds(HIGH, HIGH, LOW);
//    else if (pumpOn)                setLeds(LOW, HIGH, LOW);
//    else if (level < THRESHOLD_ON)  setLeds(HIGH, LOW, LOW);
//    else                            setLeds(LOW, LOW, HIGH);
//  }
//
//  if (fault && now - lastBeep >= 3000) {
//    lastBeep = now;
//    beep(3, 150);
//  }
//}
