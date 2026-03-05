#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * Lab 2: RFID MFRC522 Read/Write with LED feedback.
 * Pins for Arduino Uno:
 * RST: 9, SS (SDA): 10, MOSI: 11, MISO: 12, SCK: 13
 * LEDs: Green: 7, White: 8
 */

#define RST_PIN 9
#define SS_PIN 10
#define GREEN_LED 7
#define WHITE_LED 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  // Default state: White LED ON
  digitalWrite(WHITE_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;

  Serial.println("System Ready: White LED ON");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  Serial.print("Card detected! UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Mode: Programming/Writing (White LED blinks)
  Serial.println("Programming card... White LED blinking");
  for (int i = 0; i < 6; i++) {
    digitalWrite(WHITE_LED, !digitalRead(WHITE_LED));
    delay(200);
  }

  byte block = 4;
  byte dataBlock[] = "LPNU_LAB_2026_OK";
  MFRC522::StatusCode status;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block,
                                    &key, &(mfrc522.uid));
  if (status == MFRC522::STATUS_OK) {
    status = mfrc522.MIFARE_Write(block, dataBlock, 16);
    if (status == MFRC522::STATUS_OK) {
      Serial.println("Write SUCCESS! Switching to Green LED for 5 sec");

      digitalWrite(WHITE_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      delay(5000);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(WHITE_LED, HIGH);

    } else {
      Serial.print("Write failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      digitalWrite(WHITE_LED, HIGH);
    }
  } else {
    Serial.print("Auth failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(WHITE_LED, HIGH);
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}
