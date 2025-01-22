#include <time.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define FOTOREZISTOR_PIN 2  
#define PIR_PIN 3

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Simboluri pentru cărți de joc
byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};
byte diamond[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};
byte spades[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000,
};
byte club[8] = {
  B00000,
  B01110,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000,
};

unsigned long lastScreenChangeTime = 0;
const long screenChangeInterval = 5000;
bool currentScreenPlayer = true;
const int audioPin = A3;

int playerScore = 0;
int dealerScore = 0;
int playerMoney = 100;  
int betAmount = 0;

// Structură pentru a stoca o carte (valoare și simbol)
struct Card {
  int value;
  int symbol;
};

// Pachet de cărți
Card deck[52];
int cardsLeft = 52;

// Vectori pentru a stoca cărțile jucătorului și a dealerului
Card playerCards[10];
Card dealerCards[10];
int playerCardCount = 0;
int dealerCardCount = 0;

bool trebuieSaDamHit = false;  
bool trebuieSaDamStand = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(FOTOREZISTOR_PIN, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(FOTOREZISTOR_PIN), handleInterrupt, CHANGE);

  pinMode(PIR_PIN, INPUT);  
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), handleStandInterrupt, RISING);

  lcd.createChar(0, heart);
  lcd.createChar(1, spades);
  lcd.createChar(2, diamond);
  lcd.createChar(3, club);

  lcd.setCursor(3, 0);
  lcd.print("Blackjack!");
  delay(2000);
  lcd.clear();

  srand(time(NULL));
  randomSeed(analogRead(0));
  resetDeck();
}

void loop() {
  if (betAmount == 0) {
    placeBetWithAudio();  
  }
  randomSeed(millis());  // Folosesc millis() pentru a genera un număr unic în fiecare rundă
  resetDeck();
  if (playerScore == 0) {
    playerScore = dealCard(playerCards, &playerCardCount, playerScore);
    dealerScore = dealCard(dealerCards, &dealerCardCount, dealerScore);
    playerScore = dealCard(playerCards, &playerCardCount, playerScore);
  }
  // Alternează ecranele
  unsigned long currentTime = millis();
  if (currentTime - lastScreenChangeTime >= screenChangeInterval) {
    lastScreenChangeTime = currentTime;
    currentScreenPlayer = !currentScreenPlayer;

    if (currentScreenPlayer) {
      displayPlayerScreen(playerScore, playerCards, playerCardCount);
    } else {
      displayDealerScreen(dealerScore, dealerCards, dealerCardCount);
    }
  }

  if (currentScreenPlayer) {

    if (trebuieSaDamHit) {
      playerScore = dealCard(playerCards, &playerCardCount, playerScore);
      playerScore = adjustAceValue(playerScore, playerCards, playerCardCount);
      displayPlayerScreen(playerScore, playerCards, playerCardCount);
      currentScreenPlayer = false;
      trebuieSaDamHit = false;  
    }
    if (trebuieSaDamStand) {
      trebuieSaDamStand = false;
      playDealerTurn(); 
    }
    /*} else if ((input == 'D' || input == 'd') && playerCardCount == 2 && playerMoney >= betAmount) {
        // Dublare: dublează pariul și primește o singură carte
        playerMoney -= betAmount;
        betAmount *= 2; 
        playerScore = dealCard(playerCards, &playerCardCount, playerScore);
        playerScore = adjustAceValue(playerScore, playerCards, playerCardCount);
        displayPlayerScreen(playerScore, playerCards, playerCardCount);
        currentScreenPlayer = false;
        playDealerTurn();
        //functional dar n-am mai avut idei cand sa se declanseze 
      }*/

    if (playerScore > 21) {
      displayResult("Ai pierdut!", playerCards, playerCardCount, dealerCards, dealerCardCount);
      resetGame();
    }
  }
}

// Funcția de intrerupere pentru hit
void handleInterrupt() {
  int fotorezistorValue = digitalRead(FOTOREZISTOR_PIN);
  if (fotorezistorValue == LOW) {
    trebuieSaDamHit = true;
  } else {
    trebuieSaDamHit = false;
  }
}
void handleStandInterrupt() {
  trebuieSaDamStand = true;  // Setează la true când se detectează mișcare
  delay(1000);
}


void placeBetWithAudio() {
  lcd.clear();
  lcd.print("Bani: " + String(playerMoney));
  lcd.setCursor(0, 1);
  lcd.print("Alegeti pariul");  

  delay(1000);  

  unsigned long startTime = millis();
  int maxAudioValue = 0;

  while (millis() - startTime < 5000) {  
    int audioValue = analogRead(audioPin);
    if (audioValue > maxAudioValue) {
      maxAudioValue = audioValue;
    }
  }

  // Determin pariul în funcție de valoarea maximă
  if (maxAudioValue < 600) {
    betAmount = 1;
  } else if (maxAudioValue < 700) {
    betAmount = 2;
  } else if (maxAudioValue < 800) {
    betAmount = 3;
  } else if (maxAudioValue < 900) {
    betAmount = 4;
  } else {
    betAmount = 5;
  }

  if (betAmount > playerMoney) {
    betAmount = playerMoney;  
  }

  playerMoney -= betAmount;
  lcd.clear();
  lcd.print("Pariati: " + String(betAmount));
  delay(1000);
}


void resetDeck() {
  cardsLeft = 52;
  int cardIndex = 0;
  for (int suit = 0; suit < 4; suit++) {
    for (int value = 1; value <= 13; value++) {
      deck[cardIndex].value = value;
      deck[cardIndex].symbol = suit;
      cardIndex++;
    }
  }
}

int dealCard(Card *cards, int *cardCount, int score) {
  if (cardsLeft == 0) {
    resetDeck();
  }

  int randomIndex = random(cardsLeft);
  Card drawnCard = deck[randomIndex];
  deck[randomIndex] = deck[cardsLeft - 1];
  cardsLeft--;

  cards[*cardCount] = drawnCard;
  (*cardCount)++;

  int cardValue = drawnCard.value;
  if (cardValue == 1) {
    cardValue = (score <= 10) ? 11 : 1;
  } else if (cardValue >= 11 && cardValue <= 13) {
    cardValue = 10;
  }

  return score + cardValue;
}

int adjustAceValue(int score, Card *cards, int cardCount) {
  if (score > 21) {
    for (int i = 0; i < cardCount; i++) {
      if (cards[i].value == 11) {
        cards[i].value = 1;
        score -= 10;
        break;
      }
    }
  }
  return score;
}

void playDealerTurn() {
  while (dealerScore < 17) {
    dealerScore = dealCard(dealerCards, &dealerCardCount, dealerScore);
    dealerScore = adjustAceValue(dealerScore, dealerCards, dealerCardCount);
    displayDealerScreen(dealerScore, dealerCards, dealerCardCount);
    delay(1000);
  }

  determineWinner();
}

void determineWinner() {
  if (playerScore > 21) {
    displayResult("Ai pierdut!", playerCards, playerCardCount, dealerCards, dealerCardCount);
    delay(2000);
  } else if (dealerScore > 21 || (playerScore <= 21 && playerScore > dealerScore)) {
    playerMoney += betAmount * 2;
    displayResult("Ai castigat!", playerCards, playerCardCount, dealerCards, dealerCardCount);
    delay(2000);
  } else if (dealerScore == playerScore && playerScore <= 21) {
    playerMoney += betAmount;
    displayResult("Egalitate!", playerCards, playerCardCount, dealerCards, dealerCardCount);
    delay(2000);
  } else {
    displayResult("Ai pierdut!", playerCards, playerCardCount, dealerCards, dealerCardCount);
    delay(2000);
  }
  delay(3000);
  resetGame();
}

void displayPlayerScreen(int playerScore, Card *cards, int cardCount) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tu: " + String(playerScore));
  lcd.setCursor(0, 1);
  displayCards(cards, cardCount);
}

void displayDealerScreen(int dealerScore, Card *cards, int cardCount) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dealer: " + String(dealerScore));
  lcd.setCursor(0, 1);
  displayCards(cards, cardCount);
}

void displayResult(String message, Card *playerCards, int playerCardCount, Card *dealerCards, int dealerCardCount) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tu: " + String(playerScore) + " Dl: " + String(dealerScore));
  lcd.setCursor(0, 1);
  displayCards(playerCards, playerCardCount);
  lcd.setCursor(0, 2);
  displayCards(dealerCards, dealerCardCount);
  lcd.setCursor(0, 3);
  lcd.print(message);
  delay(2000);
}

void displayCards(Card *cards, int cardCount) {
  int lcdCol = 0;

  for (int i = 0; i < cardCount && lcdCol < LCD_COLS - 2; i++) {
    lcd.write((uint8_t)cards[i].symbol);
    lcdCol++;

    if (cards[i].value >= 2 && cards[i].value <= 10) {
      lcd.print(cards[i].value);
      lcdCol += String(cards[i].value).length();
    } else if (cards[i].value == 1) {
      lcd.print("A");
      lcdCol++;
    } else if (cards[i].value == 11) {
      lcd.print("J");
      lcdCol++;
    } else if (cards[i].value == 12) {
      lcd.print("Q");
      lcdCol++;
    } else if (cards[i].value == 13) {
      lcd.print("K");
      lcdCol++;
    }

    lcd.print(" ");
    lcdCol++;
  }
}

void resetGame() {
  playerScore = 0;
  dealerScore = 0;
  playerCardCount = 0;
  dealerCardCount = 0;
  betAmount = 0;
  trebuieSaDamStand = false;
}

// const int senzorMiscarePin = 3; // Portul digital la care este conectat senzorul de miscare

// void setup() {
//   Serial.begin(9600); // Initializeaza comunicarea seriala
//   pinMode(senzorMiscarePin, INPUT); // Seteaza pinul ca intrare
// }

// void loop() {
//   int valoareaCitita = digitalRead(senzorMiscarePin); // Citeste valoarea digitala a senzorului de miscare
//   if (valoareaCitita == HIGH) { // Daca senzorul detecteaza miscare
//     Serial.println("Miscare detectata!"); // Afiseaza mesajul pe monitorul serial
//   }
//   else Serial.println("Pauza");
//   delay(1000); // Pauza de 100 milisecunde
// }