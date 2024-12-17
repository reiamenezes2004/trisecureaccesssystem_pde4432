#include <Keypad.h>
#include <Servo.h>

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; // Row pins
byte colPins[COLS] = {5, 4, 3, 2}; // Column pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo setup
Servo myServo;
int servoPin = 13;

// RGB LED pins
const int redPin = A0;
const int greenPin = A1;

// Buzzer pin
const int buzzerPin = A2;

// Ultrasonic sensor pins
const int trigPin = 11;
const int echoPin = 12;

// Password setup
const char correctPassword[] = "1326";
char enteredPassword[5] = "";
int position = 0;
int attemptCounter = 0;

// RFID UIDs
const String cardUID = "23 24 48 DA";
const String tagUID = "2B CD 1B 2F";

// Ultrasonic detection thresholds
const int detectionDistance = 50;  // Distance in cm for intrusion detection

// Timer variables
unsigned long lastDistanceCheck = 0;
const unsigned long distanceCheckInterval = 500;

// Lockdown flag
bool lockdownMode = false;
bool accessGranted = false;

void setup() {
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(buzzerPin, LOW);

  myServo.attach(servoPin);
  myServo.write(0);

  Serial.println("Welcome to TriSecure!");
}

void loop() {
  if (lockdownMode) {
    triggerContinuousAlarm();
    return;
  }

  // Check for RFID card
  if (Serial.available() > 0) {
    String receivedUID = Serial.readStringUntil('\n').trim();

    if (receivedUID == cardUID) {
      grantAccess();
      return;
    } else if (receivedUID == tagUID) {
      deactivateLockdown();
      return;
    } else {
      Serial.println("Unknown UID detected.");
    }
  }

  // Handle keypad input
  char key = keypad.getKey();
  if (key) {
    if (position < 4) {
      enteredPassword[position++] = key;
    }

    if (position == 4) {
      enteredPassword[position] = '\0';

      if (strcmp(enteredPassword, correctPassword) == 0) {
        grantAccess();
      } else {
        attemptCounter++;
        indicateDeniedAccess();
        if (attemptCounter >= 3) {
          activateLockdown();
        }
      }
      clearPassword();
    }
  }

  if (millis() - lastDistanceCheck >= distanceCheckInterval) {
    lastDistanceCheck = millis();
    if (detectIntrusion()) {
      activateLockdown();
    }
  }
}

void grantAccess() {
  myServo.write(90);            // Unlock position (Barrier Open)
  digitalWrite(greenPin, HIGH); // Turn on Green LED
  accessGranted = true;

  delay(5000);                  // Keep the barrier open for 5 seconds

  myServo.write(0);             // Lock position (Barrier Closed)
  digitalWrite(greenPin, LOW);  // Turn off Green LED
  accessGranted = false;        // Reset access flag
  attemptCounter = 0;
}

void activateLockdown() {
  Serial.println("Intruder Detected! Lockdown Mode Activated!");
  lockdownMode = true;
}

void deactivateLockdown() {
  Serial.println("Lockdown deactivated by security.");
  lockdownMode = false;
  digitalWrite(redPin, LOW);
  digitalWrite(buzzerPin, LOW);
}

void triggerContinuousAlarm() {
  digitalWrite(redPin, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(redPin, LOW);
  digitalWrite(buzzerPin, LOW);
  delay(100);
}

void indicateDeniedAccess() {
  digitalWrite(redPin, HIGH);
  delay(2000);
  digitalWrite(redPin, LOW);
}

bool detectIntrusion() {
  long duration = measureDistance();
  int distance = duration / 58.2;

  if (distance > 0 && distance <= detectionDistance) {
    Serial.print("Intruder detected! Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    return true;
  }
  return false;
}

long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) {
    return 9999;
  }
  return duration;
}

void clearPassword() {
  for (int i = 0; i < 5; i++) {
    enteredPassword[i] = '\0';
  }
  position = 0;
}
