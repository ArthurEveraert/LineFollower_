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
- Werkt de robot niet terwijl de batterijen vol zijn, controleer dan de zekering (5x20 mm, 0,5 A). Vervang ze alleen door een zekering met dezelfde waarde.

### verbinding maken
De robot maakt zelf een WiFi-netwerk aan. Je hebt geen internet nodig.

1. Schakel de robot in.
2. Open de WiFi-instellingen op je smartphone of laptop.
3. Kies het netwerk **LineFollower** en vul het wachtwoord **linefollower** in.
4. Open de browser en ga naar **http://192.168.4.1**. De bedieningspagina verschijnt.

Zolang je verbonden bent, blijft de pagina de status van de robot elke 300 ms verversen.

### bediening op de webpagina
- **Start** en **Stop**: laat de robot de lijn volgen of stopt hem. De robot start alleen als hij gekalibreerd is.
- **Stap 1: kalibreer wit** en **Stap 2: kalibreer zwart**: zie de sectie kalibratie hieronder.
- **Sensoren**: per sensor de referentie voor wit en zwart, de huidige ruwe waarde, de genormaliseerde waarde (0 = wit, 1000 = zwart) en of de sensor de lijn ziet.
- **Instellingen**: schuifregelaars voor basissnelheid, maximale snelheid, gevoeligheid (drempel), witreferentie, zwartreferentie, KP, KI en KD. Elke wijziging wordt meteen opgeslagen.

### drukknop
Een drukknop op de robot start en stopt de robot. Druk één keer om te starten en nog eens om te stoppen. Dezelfde actie kan je ook op de webpagina doen.

De drukknop is aangesloten op pin **D4** van de Arduino Nano ESP32.
De indicator-LED (pin **D10**) brandt zolang de robot rijdt.

Fysieke locatie van de knop op de robot: nog in te vullen.

### kalibratie
De 8 IR-sensoren moeten gekalibreerd worden voor de ondergrond en de verlichting. Doe dit telkens op een nieuw parcours of bij andere lichtomstandigheden. De robot moet stilstaan tijdens het kalibreren.

1. Maak verbinding met de robot en open de webpagina.
2. Plaats de robot zo dat alle 8 sensoren boven het **witte** papier staan. Klik op **Stap 1: kalibreer wit**.
3. Plaats de robot zo dat alle 8 sensoren boven de **zwarte** lijn staan. Klik op **Stap 2: kalibreer zwart**.
4. De melding onder *Kalibratie* moet "Kalibratie OK. Je kunt starten." worden. Controleer in de tabel dat alle sensoren "OK" tonen.

Als een sensor "kalibratie fout" toont, herhaal dan beide stappen.

De kalibratiewaarden worden opgeslagen in het geheugen van de Arduino en blijven behouden na uitschakelen.

### standaardinstellingen
De robot is ingesteld met de volgende waarden. Ze staan zo in de code en zijn de standaard. Op de robot kunnen andere waarden zijn opgeslagen, die je op de webpagina ziet.

| parameter | standaardwaarde | bereik |
|-----------|-----------------|--------|
| basissnelheid | 150 | 0..255 |
| maximale snelheid | 230 | 0..255 |
| gevoeligheid (drempel) | 500 | 0..1000 |
| witreferentie (bijstelling) | 0 | -800..800 |
| zwartreferentie (bijstelling) | 0 | -800..800 |
| KP | 12 | 0..40 |
| KI | 0 | 0..1 |
| KD | 6 | 0..30 |

De regelcyclus is vast op 5 ms (5000 µs) en niet instelbaar.

### seriële monitor (optioneel)
De robot stuurt elke 500 ms de sensorwaarden naar de seriële monitor op 115200 baud. Dit is handig om te zien wat de sensoren meten.

### veiligheid
- De robot stopt automatisch als de lijn 1,5 seconde niet gezien wordt.
- Start de robot nooit met handen of voeten in de buurt van de wielen.
