// Interrupt proof of concept: start/stop met een drukknop op D4 (tegen GND)
// Board: Arduino Nano ESP32

const uint8_t BUTTON_PIN = D4;
const uint8_t LED_PIN = D10;             // brandt als "gestart"
const unsigned long DEBOUNCE_MS = 200;

volatile bool buttonFlag = false;        // gezet in de interrupt, verwerkt in loop()
bool running = false;
unsigned long lastPressMs = 0;

void IRAM_ATTR onButton() {
  buttonFlag = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButton, FALLING);
  Serial.println("Interrupt test klaar. Druk op de knop.");
}

void loop() {
  if (buttonFlag) {
    buttonFlag = false;
    if (millis() - lastPressMs > DEBOUNCE_MS) {
      lastPressMs = millis();
      running = !running;
      digitalWrite(LED_PIN, running ? HIGH : LOW);
      Serial.println(running ? "START" : "STOP");
    }
  }
}
