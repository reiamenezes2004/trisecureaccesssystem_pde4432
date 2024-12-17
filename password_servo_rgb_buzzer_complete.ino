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

byte rowPins[ROWS] = {12, 11, A0, A1}; // Row pins
byte colPins[COLS] = {A2, A3, A4, A5}; // Column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// RGB LED setup
int redPin = 2;    // Red pin of RGB LED
int greenPin = 3;  // Green pin of RGB LED

// Buzzer setup
int buzzerPin = 4; // Buzzer connected to pin 4

// Servo setup
Servo myServo;
int servoPin = 13; // Servo connected to pin 13

// Password setup
const char correctPassword[] = "1326"; // Correct password
char enteredPassword[5]; // Store entered password (4 digits + null terminator)
int position = 0;        // Tracks position of entered digits
int attemptCounter = 0;  // Tracks incorrect attempts

void setup() {
  Serial.begin(9600);

  // Initialize RGB LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  digitalWrite(redPin, LOW);   // Ensure red LED is off initially
  digitalWrite(greenPin, LOW); // Ensure green LED is off initially

  // Initialize buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Ensure buzzer is off initially

  // Initialize servo
  myServo.attach(servoPin);
  myServo.write(0); // Start servo at locked position (0 degrees)
}

void loop() {
  char key = keypad.getKey(); // Get keypress from keypad

  if (key) { // If a key is pressed
    Serial.print("Key Pressed: ");
    Serial.println(key);

    // Store the key in the enteredPassword array
    enteredPassword[position] = key;
    position++;

    // Check if the password is complete
    if (position == 4) {
      enteredPassword[position] = '\0'; // Null-terminate the string

      // Validate password
      if (strcmp(enteredPassword, correctPassword) == 0) {
        // Password is correct
        Serial.println("Access Granted!");

        // Turn on green LED
        delay(1000);
        digitalWrite(greenPin, HIGH);
        digitalWrite(redPin, LOW);
        delay(2000);

        // Unlock servo
        myServo.write(90); // Unlock position (90 degrees)
        delay(3000);       // Keep it unlocked for 5 seconds
        myServo.write(0);  // Lock again (0 degrees)
        digitalWrite(greenPin, LOW); // Turn off green LED
        attemptCounter = 0; // Reset incorrect attempts
      } else {
        // Password is incorrect
        Serial.println("Access Denied!");
        attemptCounter++; // Increment incorrect attempt counter

        // Turn on red LED
        delay(1000);
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, LOW);

        delay(2000); // Keep red LED on for 2 seconds
        digitalWrite(redPin, LOW); // Turn off red LED

        // Trigger buzzer on third incorrect attempt
        if (attemptCounter >= 3) {
          Serial.println("Intruder");

          // Turn on red LED and keep it on
          digitalWrite(redPin, HIGH);

          // Activate buzzer with a pattern
          for (int i = 0; i < 5; i++) { // Buzz 5 times
            digitalWrite(buzzerPin, HIGH);
            delay(300);
            digitalWrite(buzzerPin, LOW);
            delay(300);
          }

          // Reset the system after buzzer alert
          digitalWrite(redPin, LOW); // Turn off red LED
          attemptCounter = 0; // Reset incorrect attempts
        }
      }

      // Reset for the next attempt
      position = 0; // Reset position
    }
  }
}
