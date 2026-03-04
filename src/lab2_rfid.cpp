#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

/**
 * Lab 2: RFID MFRC522 Read/Write correctly.
 * Pins for Arduino Uno:
 * RST: 9
 * SS (SDA): 10
 * MOSI: 11
 * MISO: 12
 * SCK: 13
 */

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Default key is FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = 0xFF;

  Serial.println("Ready to scan RFID tags...");
}

void loop() {
  // Reset the loop if no new card is present on the sensor/reader.
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  // Show UID
  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Block to write/read (MIFARE Classic 1K has 64 blocks)
  byte block = 4;
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;

  // Authenticate using key A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block,
                                    &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Auth failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Read current data
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status == MFRC522::STATUS_OK) {
    Serial.print("Data in block 4: ");
    for (byte i = 0; i < 16; i++)
      Serial.write(buffer[i]);
    Serial.println();
  }

  // Write new data
  byte dataBlock[] = "LPNU_LAB_2026_OK"; // 16 bytes exactly
  status = mfrc522.MIFARE_Write(block, dataBlock, 16);
  if (status == MFRC522::STATUS_OK) {
    Serial.println("Data written successfully!");
  } else {
    Serial.print("Write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  delay(2000);
}
