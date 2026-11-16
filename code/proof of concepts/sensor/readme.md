# Sensoren proof of concept

minimale hard- en software die aantoont dat minimaal 6 sensoren onafhankelijk van elkaar kunnen uitgelezen worden (geen calibratie, normalisatie of interpolatie). Hierbij moet een zo groot mogelijk bereik van de AD converter benut worden (indien van toepassing)

## schema

Deze proof of concept gebruikt een **ESP32 DevKitV1** (zie [datasheet](../../../datasheets/ESP32-NodeMCU.pdf)) en 6 QTR-8A-sensoren. Het finale programma gebruikt de Arduino Nano ESP32 met 8 sensoren, zie [finaal](../../finaal/readme.md).

| functie | ESP32-pin |
|---------|-----------|
| sensor 1 | GPIO 36 |
| sensor 2 | GPIO 39 |
| sensor 3 | GPIO 34 |
| sensor 4 | GPIO 35 |
| sensor 5 | GPIO 32 |
| sensor 6 | GPIO 33 |
| IR-LEDs (aan/uit) | GPIO 23 (HIGH = aan) |

De sensoren gebruiken alleen ADC1-pinnen. ADC2 is niet bruikbaar als WiFi aan staat.

## minimale code

[Robot_Project_Sensor_6Sensoren.ino](Robot_Project_Sensor_6Sensoren.ino)

Het programma zet de IR-LEDs aan, stelt op elke pin de 11 dB-attenuatie in (meetbereik ongeveer 0 tot 3,1 V) en print elke 2,5 seconden de ruwe waarden van de zes sensoren (0 tot 4095).

## stappenplan

1. Sluit de zes sensoren aan op de pinnen hierboven en de voeding op 3,3 V of 5 V volgens de sensorprint.
2. Sluit de ESP32 DevKitV1 aan op de laptop via USB.
3. Open `Robot_Project_Sensor_6Sensoren.ino` in de Arduino IDE en kies het bord **ESP32 Dev Module**.
4. Upload het programma en open de seriële monitor op 115200 baud.
5. Houd een wit vlak voor de sensoren en noteer de waarden.
6. Houd een zwart vlak voor de sensoren en noteer de waarden. Het verschil tussen wit en zwart moet duidelijk zichtbaar zijn.
7. Als één sensor geen verandering geeft, controleer dan de aansluiting en de pin in de code.

## aandachtspunten

- Gebruik alleen de ADC1-pinnen (32 t/m 39) voor de sensoren. Die werken ook met WiFi of Bluetooth aan.
- Een QTR-8A heeft een analoge uitgang van 0 tot ongeveer 3,1 V. Met 11 dB-attenuatie gebruik je dat bereik optimaal.
- Dit programma kalibreert niet. Het toont enkel de ruwe waarden, zodat je ziet of de sensoren werken.
- De bill of materials bevat 8 IR-modules, maar deze test gebruikt er 6. Dat is de testopstelling, niet het finale ontwerp.
