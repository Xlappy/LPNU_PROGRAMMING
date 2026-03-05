#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * Lab 2: RFID Access Control with LED feedback.
 * Pins for Arduino Uno:
 * RST: 9, SS (SDA): 10, MOSI: 11, MISO: 12, SCK: 13
 * LEDs: RED: 6, GREEN: 7, WHITE: 8
 */

#define RST_PIN 9
#define SS_PIN 10
#define RED_LED 6
#define GREEN_LED 7
#define WHITE_LED 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// REPLACE THIS WITH YOUR WHITE CARD UID (see serial monitor)
// For example: {0xDE, 0xAD, 0xBE, 0xEF}
byte trustedUID[] = {0xDE, 0xAD, 0xBE, 0xEF};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  // Initial state: White LED ON, others OFF
  digitalWrite(WHITE_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;

  Serial.println("System Ready: Put your card near reader...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  Serial.println("\n--- New Card Detected ---");
  Serial.print("Card UID:");

  bool accessGranted = true;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);

    // Compare first 4 bytes for simplicity (classic MIFARE)
    if (i < 4 && mfrc522.uid.uidByte[i] != trustedUID[i]) {
      accessGranted = false;
    }
  }
  Serial.println();

  if (accessGranted) {
    Serial.println("STATUS: ACCESS GRANTED! Welcome.");
    digitalWrite(WHITE_LED, LOW);

    // Mode: Writing/Programming simulation (LED Blinking)
    Serial.println("ACTION: Updating card data... White LED blinking");
    for (int i = 0; i < 6; i++) {
      digitalWrite(WHITE_LED, !digitalRead(WHITE_LED));
      delay(150);
    }

    byte block = 4;
    byte dataBlock[] = "LPNU_LAB_2026_OK";
    MFRC522::StatusCode status;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block,
                                      &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      status = mfrc522.MIFARE_Write(block, dataBlock, 16);
      if (status == MFRC522::STATUS_OK) {
        Serial.println("RESULT: Data recorded successfully.");
        digitalWrite(WHITE_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        delay(5000); // Wait 5 seconds as requested
        digitalWrite(GREEN_LED, LOW);
      }
    }
    digitalWrite(WHITE_LED, HIGH);
  } else {
    Serial.println("STATUS: ACCESS DENIED! Invalid Card.");
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(3000); // Show red for 3 seconds
    digitalWrite(RED_LED, LOW);
    digitalWrite(WHITE_LED, HIGH);
  }

  Serial.println("SYSTEM: Back to IDLE state.");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}
