// Line follower robot, analoge sensoren
// Board:    Arduino Nano ESP32 (ESP32-S3, logica 3,3 V)
// Driver:   DRV8833 (2 motoren)
// Sensor:   8 analoge IR-sensoren op A0..A7
// Board in Arduino IDE: "Arduino Nano ESP32"
//
// Bediening:
//  - Drukknop op D4 (tussen D4 en GND): start/stop
//  - Webpagina: verbind met WiFi "LineFollower" (wachtwoord "linefollower"),
//    open http://192.168.4.1
//  - Kalibratie (robot stil):
//      stap 1: alle sensoren boven wit papier, klik "Kalibreer wit"
//      stap 2: alle sensoren boven de zwarte lijn, klik "Kalibreer zwart"
//  - Instellingen worden in het flash-geheugen (NVS) opgeslagen.

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// ---------- WiFi ----------
const char* AP_SSID = "LineFollower";
const char* AP_PASS = "linefollower";   // minimaal 8 tekens
WebServer server(80);
Preferences prefs;

// ---------- Pinnen ----------
// Sensor 1 (links) t/m sensor 8 (rechts), kijkend vanaf achter de robot
const uint8_t SENSOR_PINS[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const uint8_t IR_LED_PIN = D2;      // LED-aan pin van de sensorprint (HIGH = aan; controleer bij jouw sensor)

const uint8_t LEFT_IN1 = D3;        // DRV8833 IN1
const uint8_t LEFT_IN2 = D5;        // DRV8833 IN2
const uint8_t RIGHT_IN1 = D6;       // DRV8833 IN3
const uint8_t RIGHT_IN2 = D9;       // DRV8833 IN4

const uint8_t BUTTON_PIN = D4;      // start/stop drukknop (monostabiel), andere kant naar GND
const uint8_t RUN_LED_PIN = D10;    // indicator: brandt als de robot rijdt

// ---------- Vaste instellingen ----------
const unsigned long CYCLE_US = 5000;          // vaste regelcyclus: 5 ms
const unsigned long LOST_TIMEOUT_MS = 1500;   // stop als de lijn zo lang kwijt is
const unsigned long BUTTON_DEBOUNCE_MS = 200;
const int CAL_SAMPLES = 16;                   // metingen per sensor bij kalibratie
const bool CHECK_SPAN = false;                // true = controleer of wit en zwart ver genoeg uit elkaar liggen
const int MIN_SPAN = 300;                     // minimaal verschil (ruw) tussen wit en zwart per sensor (alleen bij CHECK_SPAN = true)
const bool SERIAL_DEBUG = true;               // true = waarden per sensor naar de Serial Monitor
const unsigned long SERIAL_INTERVAL_MS = 500; // hoe vaak de waarden worden afgedrukt

// Gewicht per sensor: links negatief, rechts positief
const int WEIGHT[8] = {-7, -5, -3, -1, 1, 3, 5, 7};

// ---------- Instelbare parameters (via webpagina, opgeslagen in NVS) ----------
struct Params {
  int   base;        // basissnelheid 0..255
  int   maxSpeed;    // maximale snelheid per motor 0..255
  float kp;          // proportioneel
  float ki;          // integraal
  float kd;          // differentieel
  int   threshold;   // gevoeligheid: vanaf deze genormaliseerde waarde (0..1000) ziet een sensor de lijn
  int   wOff;        // bijstelling van de witreferentie na kalibratie (ruwe eenheden)
  int   bOff;        // bijstelling van de zwartreferentie na kalibratie (ruwe eenheden)
};
Params P = {150, 230, 12.0, 0.0, 6.0, 500, 0, 0};

// ---------- Kalibratie ----------
uint16_t whiteRaw[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t blackRaw[8] = {0, 0, 0, 0, 0, 0, 0, 0};
bool whiteDone = false;   // stap 1 gedaan?
bool blackDone = false;   // stap 2 gedaan?

volatile bool running = false;
volatile bool buttonFlag = false;   // gezet door de interrupt, verwerkt in loop()
unsigned long lastButtonMs = 0;
float lastError = 0;
float integral = 0;
float lastCorrection = 0;
int   raw[8] = {0, 0, 0, 0, 0, 0, 0, 0};   // ruwe analoge waarde (0..4095)
int   norm[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // genormaliseerd: 0 = wit, 1000 = zwart
uint8_t lastBits = 0;                       // sensoren boven de drempel
unsigned long lastSeenMs = 0;
unsigned long nextCycleUs = 0;

// ---------- Opslag ----------
void loadParams() {
  prefs.begin("lf", true);
  P.base      = prefs.getInt("base", P.base);
  P.maxSpeed  = prefs.getInt("max", P.maxSpeed);
  P.kp        = prefs.getFloat("kp", P.kp);
  P.ki        = prefs.getFloat("ki", P.ki);
  P.kd        = prefs.getFloat("kd", P.kd);
  P.threshold = prefs.getInt("thr", P.threshold);
  P.wOff      = prefs.getInt("woff", 0);
  P.bOff      = prefs.getInt("boff", 0);
  whiteDone   = prefs.getBool("wdone", false);
  blackDone   = prefs.getBool("bdone", false);
  for (uint8_t i = 0; i < 8; i++) {
    char key[6];
    snprintf(key, sizeof(key), "wh%u", i);
    whiteRaw[i] = prefs.getUShort(key, whiteRaw[i]);
    snprintf(key, sizeof(key), "bk%u", i);
    blackRaw[i] = prefs.getUShort(key, blackRaw[i]);
  }
  prefs.end();
}

void saveParams() {
  prefs.begin("lf", false);
  prefs.putInt("base", P.base);
  prefs.putInt("max", P.maxSpeed);
  prefs.putFloat("kp", P.kp);
  prefs.putFloat("ki", P.ki);
  prefs.putFloat("kd", P.kd);
  prefs.putInt("thr", P.threshold);
  prefs.putInt("woff", P.wOff);
  prefs.putInt("boff", P.bOff);
  prefs.end();
}

void saveCalibration() {
  prefs.begin("lf", false);
  prefs.putBool("wdone", whiteDone);
  prefs.putBool("bdone", blackDone);
  for (uint8_t i = 0; i < 8; i++) {
    char key[6];
    snprintf(key, sizeof(key), "wh%u", i);
    prefs.putUShort(key, whiteRaw[i]);
    snprintf(key, sizeof(key), "bk%u", i);
    prefs.putUShort(key, blackRaw[i]);
  }
  prefs.end();
}

// ---------- Motoren ----------
void setMotor(uint8_t in1, uint8_t in2, int speed) {
  speed = constrain(speed, -P.maxSpeed, P.maxSpeed);
  if (speed >= 0) {
    analogWrite(in1, speed);
    analogWrite(in2, 0);
  } else {
    analogWrite(in1, 0);
    analogWrite(in2, -speed);
  }
}

void stopMotors() {
  setMotor(LEFT_IN1, LEFT_IN2, 0);
  setMotor(RIGHT_IN1, RIGHT_IN2, 0);
}

// Start of stop de robot. Geeft false terug als starten niet mag (kalibratie niet in orde).
bool setRunning(bool r) {
  if (r && !allCalibrated()) {
    Serial.println("Start geweigerd: kalibreer eerst op wit en zwart.");
    return false;
  }
  running = r;
  integral = 0;              // iTerm altijd op 0 bij (her)start
  lastSeenMs = millis();
  if (!running) stopMotors();
  digitalWrite(RUN_LED_PIN, running ? HIGH : LOW);
  return true;
}

// ---------- Interrupt: drukknop ----------
void IRAM_ATTR onButton() {
  buttonFlag = true;
}

// ---------- Kalibratie-controle ----------
// Een sensor is in orde als wit en zwart beide gemeten zijn en ver genoeg uit elkaar liggen.
bool sensorOk(uint8_t i) {
  if (!whiteDone || !blackDone) return false;
  if (!CHECK_SPAN) return true;   // controle op verschil uit
  long span = (long)blackRaw[i] - (long)whiteRaw[i];
  if (span < 0) span = -span;
  return span >= MIN_SPAN;
}

bool allCalibrated() {
  for (uint8_t i = 0; i < 8; i++) {
    if (!sensorOk(i)) return false;
  }
  return true;
}

// Tekst die in de app en in de Serial Monitor getoond wordt.
String calibrationMessage() {
  if (!whiteDone) return "Stap 1: kalibreer op wit.";
  if (!blackDone) return "Stap 2: kalibreer op zwart.";
  if (allCalibrated()) return "Kalibratie OK. Je kunt starten.";
  return "";
}

void printCalibrationSerial() {
  Serial.println();
  Serial.println("--- Kalibratie ---");
  Serial.println("Sensor   wit    zwart  verschil  status");
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("  ");
    Serial.print(i + 1);
    Serial.print(" (A");
    Serial.print(i);
    Serial.print(")  ");
    Serial.print(whiteRaw[i]);
    Serial.print("  ");
    Serial.print(blackRaw[i]);
    Serial.print("  ");
    Serial.print((int)blackRaw[i] - (int)whiteRaw[i]);
    Serial.print("     ");
    Serial.println(sensorOk(i) ? "OK" : "FOUT");
  }
  Serial.println(calibrationMessage());
  Serial.println();
}

// ---------- Sensoren ----------
// Zet de ruwe waarde om naar 0 (wit) .. 1000 (zwart), op basis van de kalibratie.
int normalise(int rawValue, uint8_t i) {
  long wv = (long)whiteRaw[i] + P.wOff;   // witreferentie incl. bijstelling
  long bv = (long)blackRaw[i] + P.bOff;   // zwartreferentie incl. bijstelling
  long span = bv - wv;
  if (span == 0) span = 1;  // gelijk: voorkom delen door nul
  long n = 1000L * ((long)rawValue - wv) / span;
  return constrain((int)n, 0, 1000);
}

// Leest alle sensoren uit. Vult raw[], norm[] en lastBits.
void readSensors() {
  uint8_t bits = 0;
  for (uint8_t i = 0; i < 8; i++) {
    raw[i] = analogRead(SENSOR_PINS[i]);
    norm[i] = normalise(raw[i], i);
    if (norm[i] > P.threshold) bits |= (1 << i);
  }
  lastBits = bits;
}

// Drukt per sensor de ruwe waarde en de genormaliseerde waarde af.
void printSensorsSerial() {
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(" (A");
    Serial.print(i);
    Serial.print("): raw ");
    Serial.print(raw[i]);
    Serial.print("  norm ");
    Serial.print(norm[i]);
    Serial.println(norm[i] > P.threshold ? "  <- lijn" : "");
  }
  Serial.println();
}

// Kalibreert alle sensoren op wit of zwart (gemiddelde van CAL_SAMPLES metingen).
// Mag alleen als de robot stilstaat, want dit duurt even.
void calibrate(bool black) {
  for (uint8_t i = 0; i < 8; i++) {
    long acc = 0;
    for (int k = 0; k < CAL_SAMPLES; k++) {
      acc += analogRead(SENSOR_PINS[i]);
      delay(2);
    }
    uint16_t avg = acc / CAL_SAMPLES;
    if (black) blackRaw[i] = avg; else whiteRaw[i] = avg;
  }
  if (black) blackDone = true; else whiteDone = true;
  P.wOff = 0;   // nieuwe kalibratie: bijstellingen terug naar 0
  P.bOff = 0;
  saveParams();
  saveCalibration();
  printCalibrationSerial();
}

// ---------- Webpagina ----------
const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="nl">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Line follower</title>
<style>
  body { font-family: sans-serif; margin: 16px; background: #f4f4f4; color: #222; }
  h1 { font-size: 22px; margin: 0 0 12px; }
  h2 { font-size: 18px; margin: 0 0 8px; }
  .card { background: #fff; border-radius: 10px; padding: 14px; margin-bottom: 12px; box-shadow: 0 1px 3px rgba(0,0,0,.1); }
  label { display: flex; justify-content: space-between; font-size: 16px; margin-top: 10px; }
  input[type=range] { width: 100%; }
  .buttons { display: flex; gap: 10px; }
  button { flex: 1; font-size: 16px; padding: 12px; border: 0; border-radius: 8px; color: #fff; background: #345; }
  #start { background: #2a9d4b; }
  #stop { background: #c0392b; }
  .small { font-size: 14px; color: #555; }
  #calmsg { font-size: 15px; font-weight: bold; }
  #calmsg.ok { color: #2a9d4b; }
  #calmsg.warn { color: #c0392b; }
  table { width: 100%; border-collapse: collapse; font-size: 14px; }
  th, td { padding: 6px 4px; border-bottom: 1px solid #ddd; text-align: left; vertical-align: middle; }
  tr.bad td { color: #c0392b; }
  .bar { display: inline-block; width: 60px; height: 10px; background: #ddd; border-radius: 5px; vertical-align: middle; margin-right: 6px; }
  .bar > div { height: 10px; background: #345; border-radius: 5px; }
  .lijn { font-weight: bold; color: #2a9d4b; }
</style>
</head>
<body>
<h1>Line follower</h1>

<div class="card">
  <div class="buttons">
    <button id="start" onclick="cmd('start')">Start</button>
    <button id="stop" onclick="cmd('stop')">Stop</button>
  </div>
  <p class="small" id="state">status: -</p>
</div>

<div class="card">
  <h2>Kalibratie</h2>
  <div class="buttons">
    <button onclick="calib('white')">Stap 1: kalibreer wit</button>
    <button onclick="calib('black')">Stap 2: kalibreer zwart</button>
  </div>
  <p id="calmsg">-</p>
</div>

<div class="card">
  <h2>Sensoren</h2>
  <table>
    <thead>
      <tr><th>Sensor</th><th>Wit</th><th>Zwart</th><th>Nu (ruw)</th><th>Waarde 0..1000</th><th>Status</th></tr>
    </thead>
    <tbody id="rows"></tbody>
  </table>
  <p class="small">Op lijn (1 = boven drempel): <span id="sensors">--------</span></p>
  <p class="small">fout: <span id="err">-</span> &nbsp; correctie: <span id="corr">-</span></p>
</div>

<div class="card">
  <h2>Instellingen</h2>
  <label>Basissnelheid <span id="v_base"></span></label>
  <input type="range" min="0" max="255" step="1" id="base" oninput="show('base')" onchange="send('base')">
  <label>Maximale snelheid <span id="v_max"></span></label>
  <input type="range" min="0" max="255" step="1" id="max" oninput="show('max')" onchange="send('max')">
  <label>Gevoeligheid lijn (drempel) <span id="v_thr"></span></label>
  <input type="range" min="0" max="1000" step="10" id="thr" oninput="show('thr')" onchange="send('thr')">
  <p class="small">Hoger = minder gevoelig. Een sensor telt als lijn vanaf deze waarde.</p>
  <p class="small"><b>Marge na kalibratie</b>: verschuift de referenties die bij de kalibratie zijn opgeslagen.</p>
  <label>Witreferentie <span id="v_woff"></span></label>
  <input type="range" min="-800" max="800" step="10" id="woff" oninput="show('woff')" onchange="send('woff')">
  <label>Zwartreferentie <span id="v_boff"></span></label>
  <input type="range" min="-800" max="800" step="10" id="boff" oninput="show('boff')" onchange="send('boff')">
  <p class="small">Witreferentie omhoog: wit blijft zekerder 0. Zwartreferentie omlaag: de lijn wordt makkelijker herkend.</p>
  <label>KP (sturen) <span id="v_kp"></span></label>
  <input type="range" min="0" max="40" step="0.5" id="kp" oninput="show('kp')" onchange="send('kp')">
  <label>KI (integraal) <span id="v_ki"></span></label>
  <input type="range" min="0" max="1" step="0.01" id="ki" oninput="show('ki')" onchange="send('ki')">
  <label>KD (demping) <span id="v_kd"></span></label>
  <input type="range" min="0" max="30" step="0.5" id="kd" oninput="show('kd')" onchange="send('kd')">
</div>

<script>
const keys = ['base', 'max', 'thr', 'woff', 'boff', 'kp', 'ki', 'kd'];
function show(k) { document.getElementById('v_' + k).textContent = document.getElementById(k).value; }
function send(k) { fetch('/set?' + k + '=' + document.getElementById(k).value); }
function cmd(c) {
  fetch('/' + c).then(r => r.text().then(t => { if (!r.ok) alert(t); }));
}
function calib(side) {
  fetch('/calib?side=' + side).then(r => r.text().then(t => alert(t)));
}
function fmt(v) { return v < 0 ? '-' : v; }

// Eerste keer: instellingen uit de robot ophalen
fetch('/values').then(r => r.json()).then(v => {
  keys.forEach(k => { document.getElementById(k).value = v[k]; show(k); });
});

// Elke 300 ms de status verversen
setInterval(() => {
  fetch('/status').then(r => r.json()).then(s => {
    document.getElementById('state').textContent = 'status: ' + (s.running ? 'rijdt' : 'gestopt');

    const msg = document.getElementById('calmsg');
    msg.textContent = s.calMsg;
    msg.className = s.calOk ? 'ok' : 'warn';

    let rows = '';
    for (let i = 0; i < 8; i++) {
      const status = !s.ok[i] ? 'kalibratie fout' : (s.line[i] ? '<span class="lijn">LIJN</span>' : 'geen lijn');
      rows += '<tr class="' + (s.ok[i] ? '' : 'bad') + '">' +
        '<td>' + (i + 1) + ' (A' + i + ')</td>' +
        '<td>' + fmt(s.white[i]) + '</td>' +
        '<td>' + fmt(s.black[i]) + '</td>' +
        '<td>' + s.raw[i] + '</td>' +
        '<td><span class="bar"><div style="width:' + (s.norm[i] / 10) + '%"></div></span>' + s.norm[i] + '</td>' +
        '<td>' + status + '</td>' +
        '</tr>';
    }
    document.getElementById('rows').innerHTML = rows;

    document.getElementById('sensors').textContent = s.sensors;
    document.getElementById('err').textContent = s.error.toFixed(2);
    document.getElementById('corr').textContent = s.correction.toFixed(1);
  }).catch(() => {});
}, 300);
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", PAGE);
}

void handleSet() {
  if (server.hasArg("base")) P.base = constrain(server.arg("base").toInt(), 0, 255);
  if (server.hasArg("max"))  P.maxSpeed = constrain(server.arg("max").toInt(), 0, 255);
  if (server.hasArg("thr"))  P.threshold = constrain(server.arg("thr").toInt(), 0, 1000);
  if (server.hasArg("woff")) P.wOff = constrain(server.arg("woff").toInt(), -800, 800);
  if (server.hasArg("boff")) P.bOff = constrain(server.arg("boff").toInt(), -800, 800);
  if (server.hasArg("kp"))   P.kp = server.arg("kp").toFloat();
  if (server.hasArg("ki"))   P.ki = server.arg("ki").toFloat();
  if (server.hasArg("kd"))   P.kd = server.arg("kd").toFloat();
  saveParams();
  server.send(200, "text/plain", "ok");
}

void handleValues() {
  String json = "{\"base\":" + String(P.base) +
                ",\"max\":" + String(P.maxSpeed) +
                ",\"thr\":" + String(P.threshold) +
                ",\"kp\":" + String(P.kp, 2) +
                ",\"ki\":" + String(P.ki, 2) +
                ",\"kd\":" + String(P.kd, 2) +
                ",\"woff\":" + String(P.wOff) +
                ",\"boff\":" + String(P.bOff) + "}";
  server.send(200, "application/json", json);
}

String jsonArray(const int* v) {
  String s = "[";
  for (uint8_t i = 0; i < 8; i++) {
    if (i > 0) s += ",";
    s += String(v[i]);
  }
  return s + "]";
}

void handleStatus() {
  // Sensor 1 t/m 8 van links naar rechts
  char sensors[9];
  int white[8], black[8], ok[8], line[8];
  for (uint8_t i = 0; i < 8; i++) {
    sensors[i] = ((lastBits >> i) & 1) ? '1' : '0';
    white[i] = whiteDone ? whiteRaw[i] + P.wOff : -1;   // effectieve referentie (incl. bijstelling)
    black[i] = blackDone ? blackRaw[i] + P.bOff : -1;
    ok[i] = sensorOk(i) ? 1 : 0;
    line[i] = norm[i] > P.threshold ? 1 : 0;
  }
  sensors[8] = '\0';

  String json = "{\"running\":" + String(running ? "true" : "false") +
                ",\"calOk\":" + String(allCalibrated() ? "true" : "false") +
                ",\"calMsg\":\"" + calibrationMessage() + "\"" +
                ",\"sensors\":\"" + String(sensors) + "\"" +
                ",\"raw\":" + jsonArray(raw) +
                ",\"norm\":" + jsonArray(norm) +
                ",\"white\":" + jsonArray(white) +
                ",\"black\":" + jsonArray(black) +
                ",\"ok\":" + jsonArray(ok) +
                ",\"line\":" + jsonArray(line) +
                ",\"error\":" + String(lastError, 2) +
                ",\"correction\":" + String(lastCorrection, 1) + "}";
  server.send(200, "application/json", json);
}

void handleStart() {
  if (!setRunning(true)) {
    server.send(409, "text/plain", "Kalibreer eerst op wit en zwart voordat je start.");
    return;
  }
  server.send(200, "text/plain", "ok");
}

void handleStop() {
  setRunning(false);
  server.send(200, "text/plain", "ok");
}

void handleCalib() {
  if (running) {
    server.send(409, "text/plain", "Stop de robot eerst voordat je kalibreert.");
    return;
  }
  String side = server.arg("side");
  String msg;
  if (side == "white") {
    calibrate(false);
    msg = "Wit opgeslagen. " + calibrationMessage();
  } else if (side == "black") {
    calibrate(true);
    msg = "Zwart opgeslagen. " + calibrationMessage();
  } else {
    server.send(400, "text/plain", "Onbekende kalibratie.");
    return;
  }
  server.send(200, "text/plain", msg);
}

// ---------- Regelcyclus (wordt elke CYCLE_US uitgevoerd) ----------
void runControlCycle() {
  // 1. Sensoren uitlezen en normaliseren
  readSensors();

  // 2. Afwijking van de lijn: gewogen gemiddelde van de sensoren boven de drempel
  long sumW = 0;   // som van (waarde * gewicht)
  long sumN = 0;   // som van waarden
  int count = 0;   // aantal sensoren boven de drempel
  for (uint8_t i = 0; i < 8; i++) {
    if (norm[i] > P.threshold) {
      sumW += (long)norm[i] * WEIGHT[i];
      sumN += norm[i];
      count++;
    }
  }

  float prevError = lastError;
  float error;
  if (count == 0) {
    // Lijn kwijt: blijf sturen in de richting waar hij het laatst was
    error = lastError;
  } else {
    // Gewogen positie. Alle sensoren op de lijn (kruising) geeft 0: rechtdoor
    error = (float)sumW / (float)sumN;
    lastSeenMs = millis();
  }
  lastError = error;

  // 3. PID-sturing
  integral = constrain(integral + error, -200, 200);
  float derivative = error - prevError;
  float correction = P.kp * error + P.ki * integral + P.kd * derivative;
  lastCorrection = correction;

  // 4. Motoren aansturen
  // Lijn links (error < 0): linkerwiel langzamer, rechterwiel sneller
  if (!running || millis() - lastSeenMs > LOST_TIMEOUT_MS) {
    stopMotors();
    integral = 0;
  } else {
    setMotor(LEFT_IN1, LEFT_IN2, P.base + (int)correction);
    setMotor(RIGHT_IN1, RIGHT_IN2, P.base - (int)correction);
  }
}

// ---------- Setup en loop ----------
void setup() {
  pinMode(IR_LED_PIN, OUTPUT);
  digitalWrite(IR_LED_PIN, HIGH);

  // Zelfde aanpak als de QTR-8A PoC: vaste 11 dB-attenuatie (meetbereik 0..3,1 V)
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(SENSOR_PINS[i], INPUT);
    analogSetPinAttenuation(SENSOR_PINS[i], ADC_11db);
  }

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  stopMotors();

  pinMode(RUN_LED_PIN, OUTPUT);
  digitalWrite(RUN_LED_PIN, LOW);

  // Drukknop: monostabiel, dus INPUT_PULLUP en reageren op de daling naar LOW
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButton, FALLING);

  loadParams();

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("WiFi aan, verbind met ");
  Serial.print(AP_SSID);
  Serial.print(" en open http://");
  Serial.println(WiFi.softAPIP());

  // Toon de opgeslagen kalibratie direct bij het opstarten
  if (whiteDone && blackDone) printCalibrationSerial();
  else Serial.println(calibrationMessage());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/values", handleValues);
  server.on("/status", handleStatus);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.on("/calib", handleCalib);
  server.begin();

  lastSeenMs = millis();
  nextCycleUs = micros();
}

void loop() {
  // Drukknop verwerken (met debounce)
  if (buttonFlag) {
    buttonFlag = false;
    if (millis() - lastButtonMs > BUTTON_DEBOUNCE_MS) {
      lastButtonMs = millis();
      setRunning(!running);
    }
  }

  // Wacht tot de volgende cyclus, en blijf intussen de webserver bedienen
  while ((long)(micros() - nextCycleUs) < 0) {
    server.handleClient();
  }
  nextCycleUs += CYCLE_US;
  if ((long)(micros() - nextCycleUs) > (long)CYCLE_US) {
    nextCycleUs = micros();  // te laat: opnieuw synchroniseren
  }

  runControlCycle();

  // Serial-uitvoer staat buiten de regelcyclus, zodat de regeling niet vertraagt
  if (SERIAL_DEBUG) {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= SERIAL_INTERVAL_MS) {
      lastPrint = millis();
      printSensorsSerial();
    }
  }
}
