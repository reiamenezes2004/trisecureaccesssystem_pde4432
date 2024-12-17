#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SDA_PIN 10

MFRC522 rfid(SDA_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Scan a card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) {
      Serial.print(":");
    }
  }
  Serial.println();
  delay(1000);
}
