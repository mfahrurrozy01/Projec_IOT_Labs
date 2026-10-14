#define PIN_SOIL    A0
#define PIN_RELAY   5
#define PIN_RED     14
#define PIN_YELLOW  12
#define PIN_GREEN   13
#define PIN_BUZZER  4

#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

const int SOIL_DRY = 800;
const int SOIL_WET = 350;

const int THRESHOLD_ON  = 35;
const int THRESHOLD_OFF = 60;

const unsigned long READ_INTERVAL = 200;
const unsigned long PUMP_MAX_TIME = 30000;
const unsigned long PUMP_COOLDOWN = 5000;

bool pumpOn = false;
unsigned long pumpStart = 0;
unsigned long pumpStop = 0;
unsigned long lastRead = 0;

int soilRaw = 0;
int soilPercent = 0;

void buzzerOff() {
  digitalWrite(PIN_BUZZER, HIGH);   
  delay(10);                        
}

void beepPattern(int times, int duration, int pause) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(duration);
    digitalWrite(PIN_BUZZER, LOW);   
    if (i < times - 1) delay(pause);
  }
  buzzerOff();  
}

void warningBeep() {
  
  beepPattern(3, 500, 200);
}

void pumpingBeep() {
  
  beepPattern(3, 150, 200);
}


int readSoilPercent() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(PIN_SOIL);
    delay(5);
  }
  int raw = sum / 10;
  soilRaw = raw;
  int pct = map(raw, SOIL_DRY, SOIL_WET, 0, 100);
  return constrain(pct, 0, 100);
}

void setLeds(bool r, bool y, bool g) {
  digitalWrite(PIN_RED, r);
  digitalWrite(PIN_YELLOW, y);
  digitalWrite(PIN_GREEN, g);
}

void pumpControl(bool on) {
  pumpOn = on;
  digitalWrite(PIN_RELAY, on ? RELAY_ON : RELAY_OFF);
  if (on) {
    pumpStart = millis();
    pumpingBeep();
  } else {
    pumpStop = millis();
    buzzerOff();  
  }
}

void setup() {
  digitalWrite(PIN_RELAY, RELAY_OFF);
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_BUZZER, LOW);   
  
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  digitalWrite(PIN_RELAY, RELAY_OFF);
  buzzerOff();
  setLeds(LOW, LOW, LOW);

  Serial.begin(115200);
  delay(500);  
  
  setLeds(HIGH, LOW, LOW); delay(300);
  setLeds(LOW, HIGH, LOW); delay(300);
  setLeds(LOW, LOW, HIGH); delay(300);
  setLeds(LOW, LOW, LOW);
  
  Serial.println("AquaSense siap.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;
    soilPercent = readSoilPercent();

    Serial.printf("Raw: %d | Kelembapan: %d%% | Pompa: %s\n", 
                  soilRaw, soilPercent, pumpOn ? "ON" : "OFF");

    if (!pumpOn) {
      bool cooldownDone = (pumpStop == 0) || (now - pumpStop >= PUMP_COOLDOWN);
      
      if (soilPercent < THRESHOLD_ON && cooldownDone) {
        setLeds(HIGH, LOW, LOW);   
        warningBeep();             
        buzzerOff();               
        delay(3000);               
        
        pumpControl(true);
      } else {
        setLeds(LOW, LOW, HIGH);   
        buzzerOff();               
      }
    } 
    else {
      if (soilPercent >= THRESHOLD_OFF || (now - pumpStart >= PUMP_MAX_TIME)) {
        pumpControl(false);        
        setLeds(LOW, LOW, HIGH);   
        buzzerOff();               
      } else {
        setLeds(LOW, HIGH, LOW);   
        buzzerOff();
      }
    }
  }
}
