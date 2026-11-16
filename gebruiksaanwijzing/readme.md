# Gebruiksaanwijzing

### opladen / vervangen batterijen
De robot werkt op 2 BAK 18650 Li-ion batterijen (2900 mAh), in serie geplaatst in 2 aparte batterijhouders op de printplaat (7,4 V nominaal, 8,4 V volledig geladen). De robot heeft geen ingebouwde laadschakeling, dus de batterijen worden apart opgeladen.

1. Schakel de robot uit en neem beide batterijen uit de houders.
2. Laad de batterijen op in een externe 18650 Li-ion lader tot 4,2 V per cel.
3. Plaats de batterijen terug en let op de polariteit: de + van de batterij komt aan de + van de houder.

Let op:
- Laat de cellen niet onder ±3,0 V zakken. Rijdt de robot merkbaar trager, laad dan de batterijen op.
- Gebruik altijd 2 gelijke cellen met dezelfde laadtoestand.
- Gebruik nooit beschadigde of opgezwollen batterijen.
- Werkt de robot niet terwijl de batterijen vol zijn, controleer dan de zekering (5x20 mm). Vervang ze alleen door een zekering met dezelfde waarde.

### draadloze communicatie
#### verbinding maken
De Arduino Nano ESP32 communiceert via Bluetooth Low Energy (BLE). De ESP32-S3 ondersteunt geen klassieke Bluetooth, dus de robot is niet zichtbaar in de gewone Bluetooth-instellingen van je smartphone of laptop.

1. Installeer de app **Serial Bluetooth Terminal** (Android) of **LightBlue** (iOS).
2. Schakel de robot in.
3. Open de app → *Devices* → tabblad **Bluetooth LE** → *Scan*.
4. Selecteer het toestel **`<naam robot>`** en maak verbinding.
5. Typ een commando en verstuur het. Elk commando wordt afgesloten met een newline (`\n`).

#### commando's
debug [on/off]: toont de huidige instellingen en sensorwaarden  
start: robot begint de lijn te volgen  
stop: robot stopt  
set cycle [µs]: cyclustijd van de regellus  
set power [0..255]: basissnelheid van de motoren  
set diff [0..1]: hoeveel het binnenste wiel vertraagt in een bocht  
set kp [0..]: proportionele factor van de PID-regelaar  
set ki [0..]: integrerende factor van de PID-regelaar  
set kd [0..]: differentiërende factor van de PID-regelaar  
calibrate black: kalibreert de sensoren op zwart  
calibrate white: kalibreert de sensoren op wit  

### kalibratie
De 8 IR-sensoren moeten gekalibreerd worden voor de ondergrond en de verlichting. Doe dit telkens op een nieuw parcours of bij andere lichtomstandigheden.

1. Maak verbinding met de robot.
2. Plaats de robot zo dat alle 8 sensoren boven de zwarte lijn staan en stuur `calibrate black`.
3. Plaats de robot zo dat alle 8 sensoren boven de witte ondergrond staan en stuur `calibrate white`.
4. Controleer met `debug on` of wit en zwart duidelijk verschillende waarden geven.

De kalibratiewaarden worden opgeslagen in het geheugen van de ESP32 en blijven behouden na uitschakelen.

### settings
De robot rijdt stabiel met volgende parameters:

| parameter | waarde |
|-----------|--------|
| cycle | `<invullen>` µs |
| power | `<invullen>` |
| diff | `<invullen>` |
| kp | `<invullen>` |
| ki | `<invullen>` |
| kd | `<invullen>` |

### start/stop button
`<locatie van de knop op de robot>`

Druk één keer op de knop om de robot te starten en nog eens om hem te stoppen. De robot kan ook gestart en gestopt worden met de commando's `start` en `stop`.
