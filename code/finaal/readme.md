# finaal programma

Het programma van de lijnvolger robot: [line_follower_analoog/line_follower_analoog.ino](line_follower_analoog/line_follower_analoog.ino)

## wat het doet

- leest 8 analoge IR-sensoren uit (A0 t/m A7, sensor 1 links, sensor 8 rechts)
- normaliseert de waarden met een witte en zwarte kalibratie: 0 = wit, 1000 = zwart
- berekent de positie van de lijn als gewogen gemiddelde van de sensoren boven de drempel
- stuurt de motoren met PID-regeling in een vaste cyclus van 5 ms
- stopt de robot als de lijn 1,5 s lang niet gezien wordt
- bevat een WiFi-webpagina om te starten, stoppen, kalibreren en instellingen aan te passen
- slaat de instellingen en kalibratie op in het flash-geheugen (NVS), zodat ze na het uitschakelen blijven staan

## pinnen

| functie | pin |
|---------|-----|
| sensoren 1 t/m 8 | A0, A1, A2, A3, A4, A5, A6, A7 |
| IR-LED van de sensorprint | D2 (HIGH = aan) |
| linker motor IN1 / IN2 | D3 / D5 |
| rechter motor IN1 / IN2 | D6 / D9 |
| start/stop drukknop | D4 (tegen GND, interrupt) |
| indicator LED "rijdt" | D10 |

## standaardwaarden

| parameter | standaard | betekenis |
|-----------|-----------|-----------|
| basissnelheid | 150 | snelheid waarmee de robot rijdt |
| maximale snelheid | 230 | begrenzing per motor |
| kp | 12 | proportionele factor |
| ki | 0 | integrerende factor |
| kd | 6 | differentiërende factor |
| drempel | 500 | vanaf deze genormaliseerde waarde ziet een sensor de lijn |
| cyclus | 5000 µs | vaste regelcyclus, niet instelbaar |

## uploaden

1. Installeer de Arduino IDE en het ESP32-boardpakket (Espressif).
2. Kies het bord **Arduino Nano ESP32**.
3. Open `line_follower_analoog.ino` en upload het programma.
4. Open de seriële monitor op 115200 baud: de sensorwaarden worden elke 500 ms afgedrukt.

De bibliotheken WiFi, WebServer en Preferences komen mee met het ESP32-boardpakket. Je hoeft er niets extra te installeren.

## bediening

Zie de [gebruiksaanwijzing](../../gebruiksaanwijzing/readme.md).

## opmerkingen

- Het programma start pas als alle sensoren gekalibreerd zijn. Kalibreer eerst op wit en daarna op zwart.
- Tijdens het kalibreren moet de robot stilstaan. Het programma weigert kalibreren terwijl de robot rijdt.
- De drukknop op D4 en de webpagina doen hetzelfde: de knop wisselt tussen start en stop.
