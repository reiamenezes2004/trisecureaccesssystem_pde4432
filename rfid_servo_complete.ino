#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// RFID Reader Pins
#define RST_PIN 9  // Reset pin
#define SDA_PIN 10 // SDA pin

MFRC522 rfid(SDA_PIN, RST_PIN);

// Servo Setup
Servo myServo;
int servoPin = 6;

// Authorized UIDs (Replace with your tag and card UIDs)
byte authorizedUIDs[][8] = {
  {0xE3, 0xF1, 0xBD, 0xE},  // Tag UID
  {0xD6, 0x53, 0x29, 0xF9} // Card UID
};
const int numAuthorizedUIDs = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

void setup() {
  Serial.begin(9600);

  // Initialize RFID reader
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place your RFID tag or card near the reader...");

  // Initialize servo
  myServo.attach(servoPin);
  myServo.write(0); // Start locked (0 degrees)
}

void loop() {
  // Check for a new card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("Card detected!");

  // Compare scanned UID with authorized UIDs
  if (isAuthorized(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("Access Granted!");
    myServo.write(90); // Unlock position (90 degrees)
    delay(5000);       // Keep unlocked for 5 seconds
    myServo.write(0);  // Lock again (0 degrees)
  } else {
    Serial.println("Access Denied!");
  }

  // Halt RFID card detection
  rfid.PICC_HaltA();
}

bool isAuthorized(byte *uid, byte uidSize) {
  for (int i = 0; i < numAuthorizedUIDs; i++) {
    bool match = true;
    for (byte j = 0; j < uidSize; j++) {
      if (uid[j] != authorizedUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return true; // Match found
    }
  }
  return false; // No match
}
