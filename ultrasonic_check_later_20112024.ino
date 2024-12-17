// Ultrasonic Sensor Pins
const int trigPin = 8;
const int echoPin = 9;

// RGB LED Pins
const int redPin = 2;    // Red pin of RGB LED
const int greenPin = 3;  // (Unused in this setup)
const int bluePin = 4;   // (Unused in this setup)

// Buzzer Pin
const int buzzerPin = 5;

// Threshold Distance (in cm)
const int thresholdDistance = 50; // Boundary distance

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Initialize Ultrasonic Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize RGB LED Pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);

  // Initialize Buzzer Pin
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  // Measure distance
  long duration = measureDistance();
  int distance = duration / 58.2; // Convert to cm
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance < thresholdDistance) {
    // Object is within boundary, trigger alarm
    triggerAlarm();
  } else {
    // Object is outside boundary, turn off alarm
    turnOffAlarm();
  }

  delay(100); // Small delay for stable readings
}

long measureDistance() {
  // Send a 10us HIGH pulse to trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the time it takes for the echo pin to go HIGH
  return pulseIn(echoPin, HIGH);
}

void triggerAlarm() {
  // Turn on buzzer
  digitalWrite(buzzerPin, HIGH);

  // Rapidly blink red LED
  digitalWrite(redPin, HIGH);
  delay(100);
  digitalWrite(redPin, LOW);
  delay(100);
}

void turnOffAlarm() {
  // Turn off buzzer and red LED
  digitalWrite(buzzerPin, LOW);
  digitalWrite(redPin, LOW);
}
