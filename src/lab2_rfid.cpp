#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * Lab 2: RFID Access Control with Auto-Learning.
 * NEW Logic:
 * 1. White LED blinks UNTIL a Master Card is scanned.
 * 2. After Master Card is set, White LED stays ON (Idle).
 * 3. Access control: Green for Master, Red for Others.
 */

#define RST_PIN 9
#define SS_PIN 10
#define RED_LED 6
#define GREEN_LED 7
#define WHITE_LED 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Self-learning vars
byte trustedUID[4];
bool hasMasterCard = false;
const char *SECRET_DATA = "LPNU_LAB_2026_OK";

// For non-blocking blinking
unsigned long lastBlinkTime = 0;
bool whiteState = LOW;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(WHITE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;

  Serial.println("\n--- RFID System Initialized ---");
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println("WAITING FOR MASTER CARD (White LED blinking)...");
}

void loop() {
  // Stage 1: Blinking while no Master Card is set
  if (!hasMasterCard) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= 500) {
      lastBlinkTime = currentMillis;
      whiteState = !whiteState;
      digitalWrite(WHITE_LED, whiteState);
    }
  }

  // Check for a card
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  Serial.println("\n--- Card Detected ---");

  // Stage 2: Learning the Master Card
  if (!hasMasterCard) {
    Serial.println("MASTER CARD LEARNED. System Online.");
    for (byte i = 0; i < 4; i++) {
      trustedUID[i] = mfrc522.uid.uidByte[i];
    }
    hasMasterCard = true;
    digitalWrite(WHITE_LED, HIGH); // Constant light now

    // Visual confirmation of learning
    for (int i = 0; i < 10; i++) {
      digitalWrite(WHITE_LED, !digitalRead(WHITE_LED));
      delay(100);
    }
    digitalWrite(WHITE_LED, HIGH);
  }

  // Stage 3: Comparison and Access Control
  bool isTrusted = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != trustedUID[i])
      isTrusted = false;
  }

  if (!isTrusted) {
    Serial.println("STATUS: [!] ACCESS DENIED");
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(2000);
    digitalWrite(RED_LED, LOW);
    digitalWrite(WHITE_LED, HIGH);
  } else {
    Serial.println("STATUS: [OK] MASTER CARD ACCESS");

    // Optional: Program the card if signature is missing
    byte block = 4;
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block,
                                      &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      status = mfrc522.MIFARE_Read(block, buffer, &size);
      bool alreadyProgrammed = (status == MFRC522::STATUS_OK &&
                                strncmp((char *)buffer, SECRET_DATA, 16) == 0);

      if (!alreadyProgrammed) {
        Serial.println("ACTION: Writing LPNU signature to Master Card...");
        status = mfrc522.MIFARE_Write(block, (byte *)SECRET_DATA, 16);
      }
    }

    digitalWrite(WHITE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    delay(5000);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(WHITE_LED, HIGH);
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(500); // Debounce
}
