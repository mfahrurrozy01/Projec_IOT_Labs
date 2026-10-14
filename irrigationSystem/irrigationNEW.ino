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

// ============================================================
// FUNGSI BUZZER - PASTIKAN OFF SECARA EKSPLESIT
// ============================================================

void buzzerOff() {
  digitalWrite(PIN_BUZZER, HIGH);   // Pastikan pin LOW
  delay(10);                        // Beri jeda kecil untuk stabilisasi
}

void beepPattern(int times, int duration, int pause) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(duration);
    digitalWrite(PIN_BUZZER, LOW);   // MATIKAN di setiap iterasi
    if (i < times - 1) delay(pause);
  }
  buzzerOff();  // Pastikan mati setelah selesai
}

void warningBeep() {
  // biiip biiip biiip (500ms each, 200ms pause)
  beepPattern(3, 500, 200);
}

void pumpingBeep() {
  // bip bip bip (150ms each, 200ms pause)
  beepPattern(3, 150, 200);
}

// ============================================================
// FUNGSI LAIN
// ============================================================

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
    buzzerOff();  // PASTIKAN BUZZER MATI
  }
}

// ============================================================
// SETUP - INISIALISASI AMAN
// ============================================================

void setup() {
  // LANGKAH 1: Set semua pin ke LOW SEBELUM pinMode
  // Ini mencegah spike HIGH saat inisialisasi
  digitalWrite(PIN_RELAY, RELAY_OFF);
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_BUZZER, LOW);   // KRITIS: Pastikan buzzer LOW dulu
  
  // LANGKAH 2: Baru set pinMode
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // LANGKAH 3: Konfirmasi sekali lagi semua LOW
  digitalWrite(PIN_RELAY, RELAY_OFF);
  buzzerOff();
  setLeds(LOW, LOW, LOW);

  Serial.begin(115200);
  delay(500);  // Tunggu serial stabil
  
  // Test LED sequence (buzzer TIDAK boleh bunyi)
  setLeds(HIGH, LOW, LOW); delay(300);
  setLeds(LOW, HIGH, LOW); delay(300);
  setLeds(LOW, LOW, HIGH); delay(300);
  setLeds(LOW, LOW, LOW);
  
  Serial.println("AquaSense siap.");
}

// ============================================================
// LOOP - SETIAP KONDISI MENGELOLA BUZZER SECARA EKSPLESIT
// ============================================================

void loop() {
  unsigned long now = millis();

  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;
    soilPercent = readSoilPercent();

    Serial.printf("Raw: %d | Kelembapan: %d%% | Pompa: %s\n", 
                  soilRaw, soilPercent, pumpOn ? "ON" : "OFF");

    // === KONDISI 1: POMPA MATI ===
    if (!pumpOn) {
      bool cooldownDone = (pumpStop == 0) || (now - pumpStop >= PUMP_COOLDOWN);
      
      if (soilPercent < THRESHOLD_ON && cooldownDone) {
        // Tanah kering - WARNING SEQUENCE
        setLeds(HIGH, LOW, LOW);   // Merah
        warningBeep();             // biiip biiip biiip
        buzzerOff();               // PASTIKAN MATI setelah beep
        delay(3000);               // Jeda 3 detik
        
        // Mulai pompa
        pumpControl(true);
      } else {
        // Tanah lembab - AMAN
        setLeds(LOW, LOW, HIGH);   // Hijau
        buzzerOff();               // PASTIKAN MATI
      }
    } 
    // === KONDISI 2: POMPA NYALA ===
    else {
      if (soilPercent >= THRESHOLD_OFF || (now - pumpStart >= PUMP_MAX_TIME)) {
        pumpControl(false);        // Matikan pompa
        setLeds(LOW, LOW, HIGH);   // Kembali hijau
        buzzerOff();               // PASTIKAN MATI
      } else {
        // Pompa masih nyala - kuning
        setLeds(LOW, HIGH, LOW);   // Kuning
        // Buzzer sudah mati setelah pumpingBeep() selesai
        buzzerOff();               // Safety: pastikan tetap mati
      }
    }
  }
}
