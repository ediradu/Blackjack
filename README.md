# Blackjack

Prezentarea pe scurt a proiectului:

- Oferă o versiune interactivă a jocului clasic de cărți, blackjack, utilizând senzori pentru a determina modul în care miza este aleasă și cum jucătorii pot paria și interacționa cu dealerul.
- Scopul este de a crea o experiență de joc unică, folosind senzori pentru a influența acțiunile jucătorilor și rezultatul jocului, oferind în același timp oportunitatea de a explora funcționarea acestor senzori într-un context creativ.
- Ideea a fost de a combina tehnologia modernă cu un joc tradițional, adăugând un strat de interactivitate.
- Este util, deoarece îi ajută pe alții să înțeleagă modul în care tehnologia poate fi aplicată pentru a îmbunătăți experiențele tradiționale.

Descriere generală:

- Joc interactiv de blackjack dezvoltat pe Arduino Uno;
- Fotorezistorul este echivalentul butonului de hit, in funcție de luminozitate se dă sau nu hit;
- Senzorul PIR detectează mișcarea pentru decizii de joc(Stand);
- Microfonul ajustează miza pe baza nivelului de zgomot;
- Un ecran LCD afișează scorul.

  Hardware Design:

    * Arduino UNO (ATmega328p)
    * Modul senzor PIR HC-SR501 (senzor de mișcare)
    * Modul cu fotorezistor
    * Modul senzor de sunet cu microfon
    * LCD 1602 cu I2C
    * Breadboard
    * Fire mamă-mamă, tată-tată
    * Cablu USB

  Utilizare pini pentru fiecare componentă:

1)LCD :
- VCC: Conectat la 5V de pe Arduino pentru alimentare.
- GND: Conectat la GND de pe Arduino pentru masă.
- SDA: Conectat la pinul A4 (SDA) de pe Arduino. Acesta este pinul de date seriale pentru comunicarea I2C.
- SCL: Conectat la pinul A5 (SCL) de pe Arduino. Acesta este pinul de ceas serial pentru comunicarea I2C.
  
2)Modul cu senzor PIR :
- VCC: Conectat la 5V de pe Arduino pentru alimentare.
- GND: Conectat la GND de pe Arduino pentru masă.
- OUT: Conectat la pinul digital 13 de pe Arduino. Acesta este pinul de intrare care va citi starea senzorului (HIGH când detectează mișcare, LOW când nu detectează)
  
3)Modul cu fotorezistor :
- VCC: Conectat la 5V de pe Arduino pentru alimentare.
- GND: Conectat la GND de pe Arduino pentru masă.
- OUT: Conectat la pinul digital 2 de pe Arduino. Acesta este pinul care va citi valoarea digitală corespunzătoare nivelului de lumină detectat de fotorezistor. De asemenea, utilizez și o întrerupere.
  
4)Modul cu senzor de sunet :
- VCC: Conectat la 5V de pe Arduino pentru alimentare.
- GND: Conectat la GND de pe Arduino pentru masă.
- OUT: Conectat la pinul analogic A3 de pe Arduino.

  Software Design:
  
  Mediu de dezvoltare : Arduino IDE

  Librării utilizate :
  
- LiquidCrystal_I2C - pentru controlul LCD-ului cu interfata I2C
- Wire - pentru comunicarea I2C
- time.h - pentru generarea de numere in ordine aleatoare

  Structura codului :

  * Card: Reprezintă o carte de joc, având câmpurile value (valoarea cărții) și symbol (simbolul cărții - inimă, caro, treflă, pică).
  * Vectorul deck: Stochează pachetul de 52 de cărți.
  * Vectorii playerCards și dealerCards: Stochează cărțile jucătorului și respectiv ale dealerului.
 
  * placeBetWithAudio: Utilizează intrarea de la senzorul audio pentru a plasa un pariu.
  * resetDeck: Resetează pachetul de cărți.
  * dealCard: Împarte o carte din pachet și actualizează scorul.
  * adjustAceValue: Ajustează valoarea unui as dacă scorul depășește 21.
  * playDealerTurn: Gestionează tura dealer-ului, împărțind cărți până când scorul dealer-ului ajunge la cel puțin 17.
  * determineWinner: Determină câștigătorul pe baza scorurilor și actualizează banii jucătorului.
  * displayPlayerScreen: Afișează scorul și cărțile jucătorului.
  * displayDealerScreen: Afișează scorul și cărțile dealer-ului.
  * displayResult: Afișează rezultatul jocului (câștig, pierdere sau egalitate) și mâinile finale ale jucătorului și dealer-ului.
  * resetGame: Resetează starea jocului pentru o rundă nouă.
