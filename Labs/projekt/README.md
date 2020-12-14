# Aplikace parkovacího asistenta

### Členové týmu

Marek Sicha, Mikoláš Straděj

### Cíle práce

Vytvořit aplikaci parkovacího asistenta pomocí dvou ultrazvukových senzorů HC SR04, která bude zobrazovat vzdálenost každého senzoru od překážky na LCD displej a posílat vzdálenost do konzole UART. Vzdálenost senzoru od překážky bude indikována pomocí množství rozsvícených Ledek pro každý senzor zvlášť. V aplikaci bude zahrnuta zvuková signalizace, která upozorní na blížící se předmět.


## Popis Software

SimulIDE: Jednoduchý simulátor elektronických obvodů v reálném čase.
Atmel Studio: Je integrované vývojové prostředí pro vývoj a ladění aplikací založených na procesorech Atmel a mikrokontolérů Atmel.

## Popis Hardware

Atmega 328p
Je 8 bitový mikrokontrolér Atmel RISC od společnosti Atmel kombinuje 32 kB ISP flash paměť s možnostmi čtení a zápisu, 1 KB EEPROM, 2 KB SRAM, 23 univerzálních I / O linek, 32 univerzálních pracovních registrů, tři flexibilní časovače / čítače s režimy porovnání, interní a externí přerušení, sériové programovatelné USART. Zařízení pracuje mezi 1,8-5,5 volty. Zařízení dosahuje propustnosti blížící se 1 MIPS na MHz

## Popis kódu

Hlavní soubor je main.c, který obsahuje:
1.	Definice použitých pinů
2.	Seznam použitých knihoven
3.	Globální proměnné
4.	Funkce main()

•	Nastavení
•	Konfigurace vstupních a výstupních pinů, nastavení jejich počáteční úrovně
•	Konfiguraci časovačů a jejich povolení přetečení
•	Konfiguraci UART a LCD displeje
•	Globální povolení přerušení

•	Nekonečnou smyčku
•	pokud Trigger_(Left, Right) = 0 pošle 10us startovací puls
•	Kontroluje, jestli už je Echo ve vysoké úrovni a restartuje čítač pro daný senzor
•	Kontroluje, jestli je Echo opět v nízké úrovni (nejprve musí projít vysokou úrovní (first_High_(Left, Right)) a vypočítá vzdálenost, kterou odešle příslušnému ISR

5.	ISR(TIMER2_OVF_vect)
•	Při každém přetečení přičítá jedničku pro každý senzor, doba přetečení je 16 us
•	Podle vzdálenosti vysílá pulzy do Audio výstupu

6.	ISR(TIMER1_OVF_vect)
•	Doba přetečení je 262 ms
•	Stará se výhradně jen o Levý senzor
•	Přijatou vzdálenost z nekonečné smyčky posílá do UARTu a zobrazuje na LCD displeji, podle vzdálenosti nastavuje počet aktivních ledek.

7.	ISR(TIMER0_OVF_vect)
•	Doba přetečení je 16 ms, ale vše se aktualizuje až po 6 přetečení tedy po 262 ms
•	Stará se výhradně o pravý senzor
•	Přijatou vzdálenost z nekonečné smyčky posílá do UARTu a zobrazuje na LCD displeji, podle vzdálenosti nastavuje počet aktivních ledek.


## Vývojový Diagram

Adresa


## Simulace, schéma zapojení

adresa


## Použité zdroje

1. https://github.com/tomas-fryza/Digital-electronics-2/tree/master/Labs
2. https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
3. https://lastminuteengineers.com/arduino-sr04-ultrasonic-sensor-tutorial/
4. https://en.wikipedia.org/wiki/ATmega328

