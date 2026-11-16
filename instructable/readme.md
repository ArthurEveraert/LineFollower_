# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.  

Gebruik de volgorde hieronder. Elke stap verwijst naar de readme van het onderdeel waar je meer details vindt.

### stap 1: bestel de componenten
bestel alle componenten uit de [bill of materials](../bill%20of%20materials/readme.md). De totale kost per robot is ongeveer € 48,15 incl. BTW.

### stap 2: test de motoren en de H-brug
Voordat je alles samenbouwt, test je de motoren met de proof of concept van de [H-brug](../code/proof%20of%20concepts/hbridge/readme.md). Als beide motoren in beide richtingen draaien, ga je verder.

### stap 3: bouw de printplaat
Bouw de elektronica op volgens de [technische tekeningen](../technische%20tekeningen/elektronisch/readme.md) (EasyEDA-bestand "plan B"). Het schema bevat in elk geval de DRV8833-module en de 18650-batterijhouders. De Arduino Nano ESP32, de sensoren en de zekeringhouder staan volgens de BOM ook op de robot, maar niet in het schema. Sluit ze aan volgens de pinnen in de [finaal readme](../code/finaal/readme.md) en de [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md).

Controleer de aansluitingen met een multimeter voordat je de batterijen plaatst.

### stap 4: sluit de sensoren aan
- De 8 IR-sensoren op A0 t/m A7, van links naar rechts gezien vanaf de achterkant van de robot.
- De IR-LED-aansturing op D2 (HIGH = aan).
- De start/stop-drukknop tussen D4 en GND.
- De indicator-LED op D10.

Controleer met de [sensor proof of concept](../code/proof%20of%20concepts/sensor/readme.md) dat de sensoren een verschil tussen wit en zwart geven.

### stap 5: monteer de motoren en wielen
Bevestig de twee N20-motoren aan het chassis en sluit ze aan op de H-brug. Controleer de draairichting: als een motor de verkeerde kant op draait, verwissel dan de twee draden van die motor.

### stap 6: installeer de Arduino IDE
1. Installeer de [Arduino IDE](https://www.arduino.cc/en/software).
2. Installeer het ESP32-boardpakket van Espressif via de Boards Manager.
3. Kies het bord **Arduino Nano ESP32**.

### stap 7: upload het programma
1. Open [line_follower_analoog.ino](../code/finaal/line_follower_analoog/line_follower_analoog.ino) in de Arduino IDE.
2. Verbind de Arduino met de laptop via USB.
3. Klik op Upload.
4. Open de seriële monitor op 115200 baud. Je ziet de sensorwaarden en het WiFi-adres.

Meer details staan in de [finaal readme](../code/finaal/readme.md).

### stap 8: plaats de batterijen
Plaats de twee 18650-cellen in de houders. Let op de polariteit en gebruik twee gelijke cellen, volledig opgeladen. Zie de [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md).

### stap 9: kalibreer de sensoren
1. Maak verbinding met het WiFi-netwerk **LineFollower** (wachtwoord **linefollower**).
2. Open **http://192.168.4.1**.
3. Zet de robot boven wit papier en klik op **Stap 1: kalibreer wit**.
4. Zet de robot boven de zwarte lijn en klik op **Stap 2: kalibreer zwart**.
5. Wacht op de melding "Kalibratie OK. Je kunt starten."

### stap 10: test op het parcours
1. Plaats de robot op een van de [parcours](../parcours/readme.md)-banen.
2. Druk op **Start** of op de drukknop.
3. Stel de basissnelheid, KP en KD bij op de webpagina tot de robot stabiel rijdt. De standaardwaarden zijn 150, 12 en 6.
4. Herhaal de kalibratie als het licht of het parcours verandert.

Zodra de robot stabiel rijdt, kun je de gekozen waarden in de [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md) onder *settings* invullen.
