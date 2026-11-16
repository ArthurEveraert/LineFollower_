# Elektronische tekeningen

## bedradingsschema

[schema.svg](schema.svg) toont alle verbindingen van de finale robot: batterij, zekering, H-brug, motoren, Arduino Nano ESP32, sensoren, knop en LED. De pinnen komen overeen met de code in [finaal](../../code/finaal/readme.md).

Gestippelde of rode punten in het schema zijn nog niet bevestigd, zie onderaan de lijst.

## netlijst

| net | van | naar |
|-----|-----|------|
| batterij + | 2× 18650 (serie) | zekering 0,5 A |
| VM | zekering | DRV8833 VM |
| GND | batterij − | DRV8833 GND, Nano ESP32 GND, sensoren, drukknop |
| motor links | DRV8833 AOUT1 / AOUT2 | motor links |
| motor rechts | DRV8833 BOUT1 / BOUT2 | motor rechts |
| links IN1 / IN2 | D3 / D5 | DRV8833 AIN1 / AIN2 |
| rechts IN1 / IN2 | D6 / D9 | DRV8833 BIN1 / BIN2 |
| sensor 1 t/m 8 | IR-lijnsensor 1 t/m 8 (1 = links) | A0 t/m A7 |
| IR-LEDs | D2 | IR-LED-aansturing sensorprint (HIGH = aan) |
| start/stop | drukknop | D4 (interne pull-up, knop naar GND) |
| indicator | D10 | LED met weerstand naar GND |

De kanaalnamen A en B van de H-brug zijn in de code gekoppeld aan links en rechts. Controleer bij het bouwen dat motor links op AOUT hangt, anders verwissel je de twee motoren in de code.

## nog te bevestigen

- **Voeding van de Nano ESP32.** De BOM bevat geen spanningsregelaar. Bepaal of de Nano via VIN met 7,4 V gevoed wordt, of via een aparte 5 V.
- **Logica-voeding van de DRV8833 (VCC) en nSLEEP.** Dit hangt af van de module die je gebruikt. In het EasyEDA-bestand staat de DRV8833-module als breakout.
- **Voeding van de IR-sensoren en de IR-LEDs.** Komt de voeding uit de Nano (3,3 V) of uit een apart punt?
- **Zekeringhouder.** Het EasyEDA-bestand bevat geen zekering. Het schema zet hem wel in de batterijlijn.

## EasyEDA-bestand

[easyeda_linefollower_planB.json](easyeda_linefollower_planB.json) is een EasyEDA-project met het schema "plan B". Het bevat de DRV8833-module, de 18650-batterijhouders (2S) en een LED. Het bestand is onvolledig: de Nano ESP32, de sensoren en de zekeringhouder ontbreken. Open het in EasyEDA om het schema te bekijken of aan te passen.

Het schema.svg is een overzicht voor het bouwen. Het is geen gedetailleerde printplaat-layout.
