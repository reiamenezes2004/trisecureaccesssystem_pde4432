#include <SPI.h>
#include <MFRC522.h>

// RFID Setup
#define RST_PIN 9  // Reset pin
#define SDA_PIN 10 // SDA pin

MFRC522 rfid(SDA_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Arduino ready...");
}

void loop() {
  // Check for new card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Read UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0"; // Add leading zero for single hex digits
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i != rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase(); // Ensure uppercase formatting

  Serial.println(uid); // Send UID to main Arduino

  // Halt RFID reader
  rfid.PICC_HaltA();
}
