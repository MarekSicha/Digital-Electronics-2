# Aplikace parkovacího asistenta

### Členové týmu

Marek Sicha, Mikoláš Straděj

### Cíle práce

Vytvořit aplikaci parkovacího asistenta pomocí dvou ultrazvukových senzorů HC SR04, která bude zobrazovat vzdálenost každého senzoru od překážky na LCD displej a posílat vzdálenost do konzole UART. Vzdálenost senzoru od překážky bude indikována pomocí množství rozsvícených Ledek pro každý senzor zvlášť. V aplikaci bude zahrnuta zvuková signalizace, která upozorní na blížící se předmět.


## Popis Software

SimulIDE: Jednoduchý simulátor elektronických obvodů v reálném čase.

Atmel Studio: Je integrované vývojové prostředí pro vývoj a ladění aplikací založených na procesorech Atmel a mikrokontolérů Atmel.

## Popis Hardware

### Atmega 328p

Je 8 bitový mikrokontrolér Atmel RISC od společnosti Atmel kombinuje 32 kB ISP flash paměť s možnostmi čtení a zápisu, 1 KB EEPROM, 2 KB SRAM, 23 univerzálních I / O linek, 32 univerzálních pracovních registrů, tři flexibilní časovače / čítače s režimy porovnání, interní a externí přerušení, sériové programovatelné USART. Zařízení pracuje mezi 1,8-5,5 volty. Zařízení dosahuje propustnosti blížící se 1 MIPS na MHz

### HC SR04

Je senzor pro měření vzdálenosti v rozsahu od 2 cm do 4m, obsahuje 4 piny: napájení (5V), zem, Trigger a Echo. Popis funkce je následující. Senzor dostane startovací aspoň 10us dlouhý puls na pin Trigger. Poté modul vyšle 8 cyklů ultrazvuku o frekvenci 40kHz a nastaví Echo pin na High. Jakmile se vyslaný signál odrazí od překážky a vrátí zpátky na Echo, tak se Echo nastaví na Low. Pokud se signál nevrátí do 38 ms, nastaví se Echo automaticky na low. Výsledná vzdálenost je pak  součinem doby vysoké úrovně Echa, rychlostí zvuku, jelikož puls musí urazit vzdálenost dvakrát je potřeba součin vydělit dvěma.

![gif](https://lastminuteengineers.com/wp-content/uploads/arduino/HC-SR04-Ultrasonic-Sensor-Working-Echo-reflected-from-Obstacle.gif)

## Popis kódu

Hlavní soubor je [main.c](main.c), který obsahuje:
1.	Definice použitých pinů
2.	Seznam použitých knihoven
3.	Globální proměnné
4.	Funkce main()

	a) Nastavení
	  -	Konfigurace vstupních a výstupních pinů, nastavení jejich počáteční úrovně
	  -	Konfiguraci časovačů a jejich povolení přetečení
	  -	Konfiguraci UART a LCD displeje
	  - Globální povolení přerušení

	b)	Nekonečnou smyčku
	  -	pokud Trigger_(Left, Right) = 0 pošle 10us startovací puls
	  -	Kontroluje, jestli už je Echo ve vysoké úrovni a restartuje čítač pro daný senzor
	  -	Kontroluje, jestli je Echo opět v nízké úrovni (nejprve musí projít vysokou úrovní (first_High_(Left, Right)) a vypočítá vzdálenost, kterou odešle příslušnému ISR

5.	ISR(TIMER2_OVF_vect)
   -	Při každém přetečení přičítá jedničku pro každý senzor, doba přetečení je 16 us
   -	Podle vzdálenosti vysílá pulzy do Audio výstupu

6.	ISR(TIMER1_OVF_vect)
   -	Doba přetečení je 262 ms
   -	Stará se výhradně jen o Levý senzor
 -Přijatou vzdálenost z nekonečné smyčky posílá do UARTu a zobrazuje na LCD displeji, podle vzdálenosti nastavuje počet aktivních ledek.

7.	ISR(TIMER0_OVF_vect)
   -	Doba přetečení je 16 ms, ale vše se aktualizuje až po 6 přetečení tedy po 262 ms
   -	Stará se výhradně o pravý senzor
   -	Přijatou vzdálenost z nekonečné smyčky posílá do UARTu a zobrazuje na LCD displeji, podle vzdálenosti nastavuje počet aktivních ledek.


## Vývojový Diagram

![Diagram](Images/flow_chart.png)


## Simulace, schéma zapojení

![Simulace](Images/Simulace.png)


## Použité zdroje

1. https://github.com/tomas-fryza/Digital-electronics-2/tree/master/Labs
2. https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
3. https://lastminuteengineers.com/arduino-sr04-ultrasonic-sensor-tutorial/
4. https://en.wikipedia.org/wiki/ATmega328

