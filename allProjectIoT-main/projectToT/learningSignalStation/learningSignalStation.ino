#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------------------------------------------------------------
// 1. Konfigurasi Pin
// ------------------------------------------------------------
#define DHTPIN 26
#define DHTTYPE DHT22
#define PIRPIN 27
#define LDRPIN 34
#define RELAYPIN 19
#define BUZZER 18

// ===== 5 LED SIGNAL BAR (URUT level 1 → 5) =====
#define LED_BAR1 13 // Merah - level 1 (paling gelap)
#define LED_BAR2 25 // Kuning - level 2
#define LED_BAR3 14 // Hijau - level 3
#define LED_BAR4 15 // Biru - level 4
#define LED_BAR5 4 // Putih - level 5 (paling terang)
#define SDA_PIN 21
#define SCL_PIN 22
#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_ADDR 0x3C

// ------------------------------------------------------------
// 2. Ambang Batas
// ------------------------------------------------------------
const float SUHU_PANAS = 32.0;
const float SUHU_DINGIN = 18.0;

// ===== 5 LEVEL LDR (kalibrasi sesuai ruangan!) =====
// LDR kecil = gelap, LDR besar = terang
const int LDR_L1_MAX = 500; // 0-500 → level 1 (gelap)
const int LDR_L2_MAX = 1200; // 501-1200 → level 2 (redup)
const int LDR_L3_MAX = 2000; // 1201-2000 → level 3 (sedang)
const int LDR_L4_MAX = 3000; // 2001-3000 → level 4 (baik)
// > 3000 → level 5 (sangat terang)
// Lampu nyala jika level <= nilai ini
const int LEVEL_LAMPU_ON = 2;

// ------------------------------------------------------------
// 3. Objek
// ------------------------------------------------------------
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
const char* AP_NAME = "SmartReadingBuddy";
const char* AP_PASSWORD = "12345678";

// ------------------------------------------------------------
// 4. Variabel Global
// ------------------------------------------------------------
String statusPeringatan = "Kondisi meja aman";
float lastTemp = 0.0;
float lastHum = 0.0;
int lastPir = 0;
int lastLdr = 0;
int signalLevel = 0; // 1-5
bool lastRelay = false;
bool adaOrang = false;
unsigned long lastBeep = 0;
bool buzzerAktif = false;
int ledBarPins[5] = {LED_BAR1, LED_BAR2, LED_BAR3, LED_BAR4, LED_BAR5};

// ------------------------------------------------------------
// 5. Halaman Dashboard
// ------------------------------------------------------------
const char HTML_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Learning Signal Station</title>
  <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600;700&display=swap" rel="stylesheet">
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Poppins', sans-serif;
      background: #0f172a;
      background-image:
      radial-gradient(at 0% 0%, rgba(56, 189, 248, 0.15) 0px, transparent 50%),
      radial-gradient(at 100% 0%, rgba(168, 85, 247, 0.15) 0px, transparent 50%),
      radial-gradient(at 100% 100%, rgba(236, 72, 153, 0.15) 0px, transparent 50%),
      radial-gradient(at 0% 100%, rgba(34, 197, 94, 0.15) 0px, transparent 50%);
      min-height: 100vh; display: flex; align-items: center; justify-content: center;
      padding: 20px; color: #f8fafc; overflow-x: hidden;
    }
    .container {
      position: relative; z-index: 1; width: 100%; max-width: 900px;
      background: rgba(30, 41, 59, 0.7); backdrop-filter: blur(16px);
      border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 28px;
      padding: 40px 32px; box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5);
    }
    header { text-align: center; margin-bottom: 30px; }
    h1 {
      font-size: 2rem; font-weight: 700;
      background: linear-gradient(to right, #38bdf8, #a855f7, #ec4899);
      -webkit-background-clip: text; -webkit-text-fill-color: transparent;
      margin-bottom: 8px;
    }
    .subtitle { font-size: 0.95rem; color: #94a3b8; font-weight: 300; }
    .alert-banner {
      display: flex; align-items: center; justify-content: center; gap: 10px;
      padding: 16px 20px; border-radius: 16px; margin-bottom: 30px;
      font-weight: 500; font-size: 1rem;
      background: rgba(34, 197, 94, 0.1); border: 1px solid rgba(34, 197, 94, 0.3);
      color: #4ade80; box-shadow: 0 0 20px rgba(34, 197, 94, 0.1);
      transition: all 0.4s ease;
    }
    .alert-banner.warning {
    background: rgba(239, 68, 68, 0.1); border-color: rgba(239, 68, 68, 0.3);
    color: #f87171; box-shadow: 0 0 20px rgba(239, 68, 68, 0.1);
    }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr)); gap: 16px; }
    .card {
      background: rgba(255, 255, 255, 0.03); border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 20px; padding: 20px 14px; text-align: center;
      transition: all 0.4s ease;
    }
    .card:hover { transform: translateY(-5px); background: rgba(255, 255, 255, 0.06); border-color: rgba(255, 255, 255, 0.15); }
    .card .icon { font-size: 1.8rem; margin-bottom: 8px; display: inline-block; }
    .card .label { font-size: 0.7rem; letter-spacing: 0.08em; text-transform: uppercase; color: #94a3b8; font-weight: 600; margin-bottom: 6px; }
    .card .value { font-size: 1.5rem; font-weight: 700; color: #f1f5f9; line-height: 1.2; }
    .card .sub { font-size: 0.75rem; color: #64748b; margin-top: 4px; }
    .value.good { color: #4ade80; }
    .value.warn { color: #fbbf24; }
    .value.bad { color: #f87171; }
    .value.info { color: #38bdf8; }
    .signal-container {
      display: flex; align-items: flex-end; justify-content: center; gap: 5px;
      height: 44px; margin-top: 8px;
    }
    .signal-bar {
      width: 9px; border-radius: 3px; background: rgba(255,255,255,0.15);
      transition: all 0.3s ease;
    }
    .signal-bar.active { background: #4ade80; box-shadow: 0 0 10px #4ade80; }
    .signal-bar.active.warn { 
      background: #fbbf24; box-shadow: 0 0 10px #fbbf24;
    }
    .signal-bar.active.bad { background: #f87171; box-shadow: 0 0 10px #f87171; }
    .status-bar { 
      margin-top: 24px; display: flex; align-items: center;
      justify-content: center; gap: 8px; font-size: 0.8rem; color: #64748b;
    }
    .status-dot { width: 8px; height: 8px; border-radius: 50%; background: #4ade80; box-shadow: 0 0 10px #4ade80; animation: blink 2s infinite; }
    @keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.4; } }
    .btn-reset {
      display: inline-block; margin-top: 16px; padding: 10px 20px;
      background: rgba(239, 68, 68, 0.15); border: 1px solid rgba(239, 68, 68, 0.4);
      color: #f87171; border-radius: 10px; font-size: 0.8rem; font-weight: 600;
      text-decoration: none; cursor: pointer; transition: all 0.3s ease;
    }
    .btn-reset:hover { background: rgba(239, 68, 68, 0.25); }
  </style>
</head>

<body>
  <div class="container">
  <header>
  <h1>Smart Learning Signal Station 📶💡</h1>
  <p class="subtitle">Dashboard Pemantauan Kondisi Meja Belajar</p>
  </header>
  <div class="alert-banner" id="banner">
  <span>✅</span> <span id="banner-text">Kondisi meja aman</span>
  </div>
  <div class="grid">
    <div class="card">
      <div class="icon">🌡️</div>
      <div class="label">Suhu</div>
      <div class="value" id="temp">-- °C</div>
      <div class="sub" id="temp-label">-</div>
    </div>
    <div class="card"><div class="icon">💧</div>
    <div class="label">Kelembapan</div><div class="value info" id="hum">-- %</div></div>
 <div class="card"><div class="icon">☀️</div><div
class="label">Cahaya</div><div class="value" id="light">--</div><div class="sub"
id="light-label">-</div></div>
 <div class="card"><div class="icon">🧑‍🎓</div><div
class="label">Kehadiran</div><div class="value" id="presence">--</div></div>
 <div class="card"><div class="icon">💡</div><div class="label">Lampu
Belajar</div><div class="value" id="relay">--</div></div>
 <div class="card">
 <div class="icon">📶</div>
 <div class="label">Learning Signal</div>
 <div class="value" id="signal">- / 5</div>
 <div class="signal-container" id="signal-bars">
 <div class="signal-bar" style="height:8px"></div>
 <div class="signal-bar" style="height:14px"></div>
 <div class="signal-bar" style="height:22px"></div>
 <div class="signal-bar" style="height:32px"></div>
 <div class="signal-bar" style="height:42px"></div>
 </div>
 </div>
 </div>
 <div class="status-bar">
 <div class="status-dot"></div>
 <span id="status">Menganalisis kondisi meja...</span>
 </div>
 <div style="text-align:center;">
 <a href="/resetwifi" class="btn-reset">🔄 Ganti WiFi</a>
 </div>
 </div>
 <script>
 const el = {
 temp: document.getElementById('temp'), tempLabel:
document.getElementById('temp-label'),
 hum: document.getElementById('hum'), light:
document.getElementById('light'),
 lightLabel: document.getElementById('light-label'), presence:
document.getElementById('presence'),
 relay: document.getElementById('relay'), banner:
document.getElementById('banner'),
 bannerText: document.getElementById('banner-text'), status:
document.getElementById('status'),
 signal: document.getElementById('signal'), signalBars:
document.querySelectorAll('#signal-bars .signal-bar')
 };
 function labelCahaya(n) {
 if (n > 3000) return { text: 'Sangat Terang', cls: 'good' };
 if (n > 2000) return { text: 'Baik', cls: 'good' };
 if (n > 1200) return { text: 'Sedang', cls: 'warn' };
 if (n > 500) return { text: 'Redup', cls: 'warn' };
 return { text: 'Gelap', cls: 'bad' };
 }
 function labelSuhu(t) {
 if (t >= 32) return { text: 'Panas', cls: 'bad' };
 if (t <= 18) return { text: 'Dingin', cls: 'info' };
 return { text: 'Normal', cls: 'good' };
 }
 async function perbaruiData() {
 try {
 const res = await fetch('/data');
 const d = await res.json();
 const s = labelSuhu(d.temperature);
 el.temp.textContent = d.temperature.toFixed(1) + ' °C';
 el.temp.className = 'value ' + s.cls;
 el.tempLabel.textContent = s.text;
 el.hum.textContent = d.humidity.toFixed(1) + ' %';
 const c = labelCahaya(d.light_raw);
 el.light.textContent = d.light_raw;
 el.light.className = 'value ' + c.cls;
 el.lightLabel.textContent = c.text;
 el.presence.textContent = d.presence ? 'Ada' : 'Tidak Ada';
 el.presence.className = 'value ' + (d.presence ? 'good' : 'bad');
  el.relay.textContent = d.relay ? 'Nyala' : 'Mati';
 el.relay.className = 'value ' + (d.relay ? 'info' : 'bad');
 // ===== SIGNAL BAR =====
 el.signal.textContent = d.signal_level + ' / 5';
 const lvl = d.signal_level;
 let barCls = 'signal-bar active';
 if (lvl <= 2) barCls = 'signal-bar active bad';
 else if (lvl === 3) barCls = 'signal-bar active warn';
 el.signalBars.forEach((bar, i) => {
 bar.className = (i < lvl) ? barCls : 'signal-bar';
 });
 if (d.alert && d.alert !== 'Kondisi meja aman') {
 el.bannerText.textContent = d.alert;
 el.banner.className = 'alert-banner warning';
 el.banner.querySelector('span').textContent = '⚠️';
 } else {
 el.bannerText.textContent = 'Kondisi meja aman';
 el.banner.className = 'alert-banner';
 el.banner.querySelector('span').textContent = '✅';
 }
 el.status.textContent = 'Terakhir diperbarui: ' + new
Date().toLocaleTimeString('id-ID');
 } catch (err) { el.status.textContent = 'Gagal: ' + err.message; }
 }
 setInterval(perbaruiData, 2000); perbaruiData();
 </script>
</body>
</html>
)=====";
// ------------------------------------------------------------
// 6. Endpoint
// ------------------------------------------------------------
void handleRoot() { server.send(200, "text/html", HTML_PAGE); }
void handleData() {
  String json = "{";
 json += "\"temperature\":" + String(lastTemp, 1) + ",";
 json += "\"humidity\":" + String(lastHum, 1) + ",";
 json += "\"presence\":" + String(lastPir) + ",";
 json += "\"light_raw\":" + String(lastLdr) + ",";
 json += "\"signal_level\":" + String(signalLevel) + ",";
 json += "\"relay\":" + String(lastRelay ? "true" : "false") + ",";
 json += "\"alert\":\"" + statusPeringatan + "\"";
 json += "}";
 server.send(200, "application/json", json);
}
// ------------------------------------------------------------
// 7. Reset WiFi
// ------------------------------------------------------------
void handleResetWiFi() {
 String page = "<html><body
style='font-family:sans-serif;background:#0f172a;color:#fff;text-align:center;pad
ding:50px;'>";
 page += "<h2>🔄 WiFi Direset!</h2>";
 page += "<p>ESP32 akan restart dan membuat Access Point
<b>SmartReadingBuddy</b>.</p>";
 page += "<p>Hubungkan HP kamu ke WiFi itu (password: <b>12345678</b>) untuk
setting ulang.</p>";
 page += "</body></html>";
 server.send(200, "text/html", page);
 delay(1000);
 WiFiManager wm;
 wm.resetSettings();
 delay(500);
 ESP.restart();
}
// ------------------------------------------------------------
// 8. Tentukan Level Signal dari LDR
// ------------------------------------------------------------
int hitungSignalLevel(int ldr) {
 if (ldr <= LDR_L1_MAX) return 1
  if (ldr <= LDR_L2_MAX) return 2;
 if (ldr <= LDR_L3_MAX) return 3;
 if (ldr <= LDR_L4_MAX) return 4;
 return 5;
}
// ------------------------------------------------------------
// 9. Update Tampilan OLED
// ------------------------------------------------------------
void updateOLED() {
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0, 0);
 display.print(adaOrang ? "Anak terdeteksi!" : "Tidak ada anak");
 display.setCursor(0, 16);
 display.print("Cahaya : ");
 display.print(lastLdr);
 display.setCursor(0, 32);
 display.print("Signal : ");
 display.print(signalLevel);
 display.print("/5 ");
 // Tampilkan mini signal bar di kanan
 int barX = 80;
 for (int i = 0; i < 5; i++) {
 if (i < signalLevel) {
 display.fillRect(barX + (i * 9), 40 - (i * 2), 7, 6 + (i * 2),
SSD1306_WHITE);
 } else {
 display.drawRect(barX + (i * 9), 40 - (i * 2), 7, 6 + (i * 2),
SSD1306_WHITE);
 }
 }
 display.setCursor(0, 48);
 display.print(lastRelay ? "Lampu: ON" : "Lampu: OFF");
 display.print(" ");
 display.print(lastTemp, 0);
 display.print("C");
 display.display();
}
// ------------------------------------------------------------
// 10. Logika Peringatan + Signal Bar + Relay
// ------------------------------------------------------------
void cekPeringatan() {
 // ===== Hitung level signal dari LDR =====
 signalLevel = hitungSignalLevel(lastLdr);
 // ===== Kontrol 5 LED Signal Bar (berurutan) =====
 for (int i = 0; i < 5; i++) {
 digitalWrite(ledBarPins[i], (i < signalLevel) ? HIGH : LOW);
 }
 // ===== Kontrol USB Lamp via Relay (hanya dari LDR) =====
 // Lampu nyala jika signal level <= 2 (gelap/redup)
 lastRelay = (signalLevel <= LEVEL_LAMPU_ON);
 digitalWrite(RELAYPIN, lastRelay ? HIGH : LOW);
 // ===== Logika Peringatan (gabungan PIR, LDR, suhu) =====
 bool cahayaOK = (signalLevel >= 3);
 bool suhuOK = (lastTemp < SUHU_PANAS && lastTemp > SUHU_DINGIN);
 if (adaOrang && !cahayaOK) {
 statusPeringatan = "Cahaya redup, lampu dinyalakan!";
 } else if (adaOrang && !suhuOK) {
 statusPeringatan = (lastTemp >= SUHU_PANAS) ? "Suhu ruangan terlalu panas!" :
"Suhu ruangan terlalu dingin!";
 } else if (!adaOrang) {
 statusPeringatan = "Tidak ada orang di meja.";
 } else {
 statusPeringatan = "Kondisi meja aman";
 }

  // ===== Buzzer =====
 bool bahaya = (adaOrang && (!cahayaOK || !suhuOK));
 buzzerAktif = bahaya;
 if (!bahaya) digitalWrite(BUZZER, LOW);
}
// ------------------------------------------------------------
// 11. Setup
// ------------------------------------------------------------
void setup() {
 Serial.begin(115200);
 delay(500);
 Serial.println("\n============================================");
 Serial.println(" SMART LEARNING SIGNAL STATION - BOOTING");
 Serial.println("============================================");
 // --- Pin Mode ---
 pinMode(PIRPIN, INPUT);
 pinMode(LDRPIN, INPUT);
 pinMode(RELAYPIN, OUTPUT);
 pinMode(BUZZER, OUTPUT);
 for (int i = 0; i < 5; i++) {
 pinMode(ledBarPins[i], OUTPUT);
 digitalWrite(ledBarPins[i], LOW);
 }
 digitalWrite(RELAYPIN, LOW);
 digitalWrite(BUZZER, LOW);
 // --- Sensor DHT ---
 dht.begin();
 // --- OLED ---
 Wire.begin(SDA_PIN, SCL_PIN);
 if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
 Serial.println("[ERROR] OLED gagal diinisialisasi!");
 } else {
  Serial.println("[OK] OLED siap.");
 }
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0, 0);
 display.println("Smart Learning");
 display.println("Signal Station");
 display.println("Booting...");
 display.display();
 // ============================================================
 // WIFIMANAGER
 // ============================================================
 WiFiManager wm;
 wm.setConfigPortalTimeout(180);
 wm.setAPCallback([](WiFiManager *myWiFiManager) {
 Serial.println("[INFO] Masuk mode config portal!");
 display.clearDisplay();
 display.setCursor(0, 0);
 display.println("Setting WiFi...");
 display.setCursor(0, 16);
 display.println("1. Connect ke:");
 display.setCursor(0, 28);
 display.println("SmartReadingBuddy");
 display.setCursor(0, 42);
 display.println("2. Buka 192.168.4.1");
 display.display();
 });
 Serial.println("\n[INFO] Mencoba konek WiFi...");
 bool connected = wm.autoConnect(AP_NAME, AP_PASSWORD);
 if (!connected) {
 Serial.println("[ERROR] Gagal konek & timeout portal!");
 display.clearDisplay();
 display.setCursor(0, 0)
  display.println("WiFi Gagal!");
 display.println("Restart...");
 display.display();
 delay(3000);
 ESP.restart();
 }
 Serial.println("\n[INFO] WiFi terhubung!");
 Serial.print("[INFO] SSID: "); Serial.println(WiFi.SSID());
 Serial.print("[INFO] IP : "); Serial.println(WiFi.localIP());
 display.clearDisplay();
 display.setCursor(0, 0);
 display.println("WiFi Connected!");
 display.setCursor(0, 16);
 display.print("IP: "); display.println(WiFi.localIP());
 display.setCursor(0, 32);
 display.print("SSID:"); display.println(WiFi.SSID());
 display.display();
 delay(2500);
 // ============================================================
 // WEB SERVER
 // ============================================================
 server.on("/", handleRoot);
 server.on("/data", handleData);
 server.on("/resetwifi", handleResetWiFi);
 server.begin();
 Serial.println("\n[INFO] Web Server berjalan.");
 Serial.print("[INFO] Buka dashboard di: http://");
 Serial.println(WiFi.localIP());
 Serial.println("============================================\n");
}
// ------------------------------------------------------------
// 12. Loop
// ------------------------------------------------------------
void loop() {
 server.handleClient();
 // ===== Baca PIR setiap 100ms (responsif terhadap aktivitas real) =====
 static unsigned long lastPirCek = 0;
 if (millis() - lastPirCek >= 100) {
 lastPirCek = millis();
 lastPir = digitalRead(PIRPIN);
 adaOrang = (lastPir == HIGH);
 }
 // ===== Baca DHT + LDR + update output setiap 500ms =====
 static unsigned long lastCek = 0;
 if (millis() - lastCek >= 500) {
 lastCek = millis();
 float t = dht.readTemperature();
 float h = dht.readHumidity();
 if (isnan(t)) t = lastTemp;
 if (isnan(h)) h = lastHum;
 lastTemp = t;
 lastHum = h;
 lastLdr = analogRead(LDRPIN);
 cekPeringatan();
 updateOLED();
 }
 // ===== Toggle buzzer =====
 if (buzzerAktif && millis() - lastBeep >= 400) {
 lastBeep = millis();
 digitalWrite(BUZZER, !digitalRead(BUZZER));
 }
 delay(10);
}

 }
}
