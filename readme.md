# LineFollower

Lijnvolger robot gebouwd rond een Arduino Nano ESP32, met twee N20-motoren, een DRV8833 H-brug en 8 IR-sensoren. De robot volgt een zwarte lijn op een wit parcours, met PID-regeling. Instellingen worden via een webpagina op de robot aangepast.

![A description of my image](images/empty.png)

  
## specifications

microcontroller: Arduino Nano ESP32 (ESP32-S3, logica 3,3 V)

motors: 2× Aslong JGA12-N20-30, DC-transmissiemotor 6 V, 500 RPM

h-bridge: DRV8833 (2 motoren)

sensors: 8× analoge IR lijn-tracking module, uitgelezen op A0 t/m A7

batteries: 2× BAK 18650 Li-ion 2900 mAh, in serie (7,4 V nominaal, 8,4 V volledig geladen), 2 batterijhouders op de printplaat, zekering 5x20 mm 0,5 A

wireless communication: WiFi access point "LineFollower", bediening via de webpagina op http://192.168.4.1

distance sensor - motors: geen (geen encoder in de bill of materials)

weight: nog niet gewogen

speed: basissnelheid standaard 150, maximaal 230 (PWM 0..255); gemeten snelheid nog niet vastgelegd

## documentatie

- [bill of materials](bill%20of%20materials/readme.md)
- [technische tekeningen](technische%20tekeningen/elektronisch/readme.md)
- [proof of concepts](code/proof%20of%20concepts/readme.md)
- [code](code/readme.md)
- [parcours](parcours/readme.md)
- [instructable](instructable/readme.md)
- [gebruiksaanwijzing](gebruiksaanwijzing/readme.md)
- [datasheets](datasheets/readme.md)
