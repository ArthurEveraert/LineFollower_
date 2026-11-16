# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat er communicatie kan gebeuren tussen de microcontroller en een laptop of smartphone, gebruik makend van WiFi en een webpagina (HTTP).
<br />
### configuratie

| instelling | waarde |
|------------|--------|
| modus | WiFi access point (de Arduino maakt zelf het netwerk) |
| SSID | `LineFollower` |
| wachtwoord | `linefollower` |
| adres | `http://192.168.4.1` |
| poort | 80 |

Bibliotheken: `WiFi` en `WebServer`, meegeleverd met het ESP32-boardpakket.

### minimale code

[Wireless_ping.ino](Wireless_ping.ino)

De webpagina stuurt een verzoek naar `/ping`. De Arduino antwoordt met `pong <teller>` en telt het aantal verzoeken. Hetzelfde patroon gebruikt het finale programma voor de knoppen Start en Stop, en voor het ophalen van de status.

### opmerkingen

- De eerste versie van de gebruiksaanwijzing ging uit van Bluetooth Low Energy. De finale robot gebruikt WiFi, omdat de bediening via een webpagina op elke telefoon en laptop werkt.
- Een access point geeft geen internettoegang. Dat is niet nodig voor de robot: het toestel en de Arduino praten rechtstreeks met elkaar.
- Het verzoek-antwoord-patroon is duplex in de praktijk: de telefoon kan op elk moment een commando sturen, en de Arduino antwoordt. De seriële monitor toont elk verzoek.

### gebruiksaanwijzing

1. Upload `Wireless_ping.ino` naar de Arduino Nano ESP32.
2. Open de seriële monitor op 115200 baud en lees het adres af.
3. Verbind je laptop of smartphone met WiFi `LineFollower`.
4. Open `http://192.168.4.1` in de browser.
5. Druk op **Ping**: de tekst verandert in `pong 1`, `pong 2`, enzovoort.

### status

De code is nog niet op hardware getest.
