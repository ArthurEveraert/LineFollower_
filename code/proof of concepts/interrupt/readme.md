# start/stop interrupt proof of concept
minimale hard- en software die de correcte werking van een start/stop drukknop aantoont, gebruik makend van een hardware interrupt

## schema

| onderdeel | aansluiting |
|-----------|-------------|
| drukknop | één kant op D4, andere kant op GND (interne pull-up aan) |
| LED (optioneel, of de ingebouwde LED op D10) | D10 |

De drukknop is monostabiel: hij geeft alleen bij indrukken een signaal.

## minimale code

[Interrupt_startstop.ino](Interrupt_startstop.ino)

De interrupt wordt geactiveerd op de dalende flank (indrukken). De interrupt-routine zet alleen een vlag. Het wisselen tussen start en stop gebeurt in `loop()`, met 200 ms debounce.

## stappenplan

1. Sluit de drukknop aan tussen D4 en GND.
2. Sluit de LED aan op D10 (eventueel met een weerstand).
3. Open `Interrupt_startstop.ino` in de Arduino IDE en kies het bord **Arduino Nano ESP32**.
4. Upload het programma en open de seriële monitor op 115200 baud.
5. Druk één keer op de knop: de monitor toont `START` en de LED gaat aan.
6. Druk nog eens: de monitor toont `STOP` en de LED gaat uit.
7. Houd de knop ingedrukt en laat los: er mag maar één wissel per indrukking plaatsvinden.

## aandachtspunten

- Zonder debounce kan één indrukking meerdere wissels geven. De 200 ms in de code voorkomt dat.
- In de interrupt-routine mag geen `Serial.print` of `delay` staan. Die gebruiken we hier niet.
- Het finale programma gebruikt dezelfde aanpak op D4, zie [finaal](../../finaal/readme.md).

## status

De code is geschreven op basis van het finale programma, maar nog niet op de robot getest.
