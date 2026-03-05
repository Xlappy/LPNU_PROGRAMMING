#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * Lab 2: RFID Access Control with smart programming.
 * White LED blinks ONLY on the first write.
 */

#define RST_PIN 9
#define SS_PIN 10
#define RED_LED 6
#define GREEN_LED 7
#define WHITE_LED 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Use serial monitor to find your UID and replace this:
byte trustedUID[] = {0xDE, 0xAD, 0xBE, 0xEF};
const char *SECRET_DATA = "LPNU_LAB_2026_OK";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(WHITE_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;

  Serial.println("System Ready: Access Control Online.");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  Serial.println("\n--- Card Scan ---");

  bool isTrusted = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != trustedUID[i])
      isTrusted = false;
  }

  if (!isTrusted) {
    Serial.println("STATUS: [!] UNKNOWN CARD - ACCESS DENIED");
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(3000);
    digitalWrite(RED_LED, LOW);
    digitalWrite(WHITE_LED, HIGH);
  } else {
    Serial.println("STATUS: [OK] TRUSTED CARD DETECTED");

    byte block = 4;
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block,
                                      &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      // Read to check if already programmed
      status = mfrc522.MIFARE_Read(block, buffer, &size);
      bool alreadyProgrammed = (status == MFRC522::STATUS_OK &&
                                strncmp((char *)buffer, SECRET_DATA, 16) == 0);

      if (!alreadyProgrammed) {
        Serial.println(
            "ACTION: New Trusted Card! Programming... (Blinking White)");
        for (int i = 0; i < 8; i++) {
          digitalWrite(WHITE_LED, !digitalRead(WHITE_LED));
          delay(150);
        }
        status = mfrc522.MIFARE_Write(block, (byte *)SECRET_DATA, 16);
        if (status == MFRC522::STATUS_OK)
          Serial.println("RESULT: Successfully programmed.");
      } else {
        Serial.println("ACTION: Card already programmed. Skipping write.");
      }

      // Access Granted sequence
      Serial.println("ACCESS GRANTED! Green Light.");
      digitalWrite(WHITE_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      delay(5000);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(WHITE_LED, HIGH);
    } else {
      Serial.print("AUTH ERROR: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}
