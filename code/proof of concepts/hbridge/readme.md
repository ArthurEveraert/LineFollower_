# H-Bridge proof of concept

minimale hard- & software + stappenplan dat aantoont dat 2 motoren onafhankelijk van elkaar kunnen draaien, en (traploos) regelbaar zijn in snelheid en draairichting.

## schema

De H-brug (DRV8833, zie bill of materials) krijgt per motor twee ingangen: `IN1` en `IN2`. Door op de ene ingang PWM aan te bieden en de andere op 0 te zetten, draait de motor vooruit of achteruit. De snelheid is de PWM-duty cycle.

| GPIO (nummer in de code) | functie | motor |
|:------------------------:|---------|-------|
| 16 | `motorA_IN1` | motor A |
| 17 | `motorA_IN2` | motor A |
| 18 | `motorB_IN1` | motor B |
| 19 | `motorB_IN2` | motor B |

Deze test gebruikt GPIO-nummers. Het finale programma gebruikt de D-labels van de Arduino Nano ESP32 op de robot: linker motor op D3 en D5, rechter motor op D6 en D9 (zie [finaal](../../finaal/readme.md)). Controleer bij het aansluiten welke GPIO bij welk D-label hoort.

Voeding en motoraansluitingen (`VM`, `GND`, `AOUT1/AOUT2`, `BOUT1/BOUT2`) staan nog niet in deze readme. Vul ze aan op basis van het schema van de robot of de datasheet van de driver.

## minimale code

[Hbrug.ccp](Hbrug.ccp)

Het programma:
- zet de vier pinnen als output
- laat motor A en daarna motor B elk vooruit en achteruit draaien, met de snelheid stapsgewijs van 50 tot 255 (en terug) in stappen van 5
- zet de andere motor tijdens elke test op stop
- logt de huidige stap via de seriële monitor (115200 baud)

## stappenplan

1. Sluit de H-brug aan volgens het schema hierboven. Let op dat de voeding van de motoren pas wordt ingeschakeld nadat alle verbindingen gecontroleerd zijn.
2. Sluit de Arduino Nano ESP32 aan op de laptop via USB.
3. Open `Hbrug.ccp` in de Arduino IDE en kies het bord **Arduino Nano ESP32**.
4. Upload het programma.
5. Open de seriële monitor op 115200 baud.
6. Controleer dat de melding `Setup klaar! Motoren test start...` verschijnt.
7. Motor A moet eerst vooruit en daarna achteruit draaien, met toenemende snelheid. Motor B blijft dan stil.
8. Daarna moet motor B hetzelfde doen, terwijl motor A stil blijft.
9. Als een motor in de verkeerde richting draait, verwissel dan de twee motordraden, of de twee IN-pinnen in de code.
10. Als een motor niet draait bij lage snelheid, is de startwaarde van 50 mogelijk te laag. Pas de waarde dan aan in de loops.

## aandachtspunten

- De twee ingangen van dezelfde motor mogen nooit tegelijk een PWM-signaal krijgen. De code zet altijd één van beide op 0.
- De motoren zijn N20 6 V-motoren (zie datasheets). Gebruik de batterijvoeding van de robot (2 × 18650 in serie, 7,4 V nominaal, 8,4 V volledig geladen) en houd de zekering van 0,5 A in de robot in het oog.
- Laat de motoren niet langdurig vastlopen. Een vastgelopen motor trekt veel stroom.
- Het programma draait oneindig in een lus. Zet de voeding uit om het te stoppen, of voeg een stopcommando toe.
