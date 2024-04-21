#include <Servo.h>
#include <Adafruit_Keypad_Ringbuffer.h>
#include <Adafruit_Keypad.h>

const byte Rows = 4;
const byte Columns = 4;

#define redLED 3
#define greenLED 4
#define motionLedPin 8

const int trigPin = 27;
const int echoPin = 25;
const int servoPin = 5;
const int motionSensorPin = 10; // Pin connected to the motion sensor

char keypadArr[Rows][Columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[Rows] = {52, 50, 48, 46};
byte columnPins[Columns] = {34, 32, 30, 26};
Servo gateServo;
Adafruit_Keypad key = Adafruit_Keypad(makeKeymap(keypadArr), rowPins, columnPins, Rows, Columns);

const String password = "4512";
char keypadChar;
String userInput;

const int distanceThreshold = 5; // Adjust as needed
bool motionDetected = false;

void setup() {
  key.begin();
  Serial.begin(9600);
  Serial.println("Press * to clear and press # to submit.");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(motionLedPin, OUTPUT);
  pinMode(motionSensorPin, INPUT);
  gateServo.attach(servoPin);
}

void loop() {
  sensorMotion();

  // Check ultrasonic sensor only if motion is not detected
  if (!motionDetected) {
    // Ultrasonic sensor part
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    if (distance < distanceThreshold) {
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      openGate();
      delay(50); // delay is for when led changes from red to green 
    } else{
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, HIGH);
      keypadGateOpening();
      delay(1000);
    }
 } //
 delay(1000);
}


void sensorMotion() {
  // Check motion sensor
  int motionState = digitalRead(motionSensorPin);

  if (motionState == HIGH) {
    digitalWrite(motionLedPin, HIGH);
    delay(500);
    digitalWrite(motionLedPin, LOW);//
    motionDetected = true;
  } else { // Turn off red LED
    motionDetected = false;
  }
}

void keypadGateOpening() {
  // Check keypad
  key.tick();
  while (key.available()) {
    keypadEvent e = key.read();
    keypadChar = (char)e.bit.KEY;
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      if (keypadChar == '*') {
        userInput = ""; // Clear the input
        Serial.println("Password Cleared.");
      } else {
        if (keypadChar == '#') {
          // Check if the password is correct
          if (userInput == password) {
            Serial.println("The Password is correct!");
            digitalWrite(greenLED, HIGH);
            digitalWrite(redLED, LOW);
            openGate();
            delay(500);
          } else {
            Serial.println("The Password is incorrect");
            digitalWrite(redLED, HIGH);
            digitalWrite(greenLED, LOW);
            openGate(); // close the gate for unauthorized access
          }
          userInput = ""; // Clear the input after checking
        } else {
          // Add the pressed key to the input
          userInput += keypadChar;
          Serial.print(keypadChar);
        }
      }
    }
  }
}

void openGate() {
  gateServo.write(90); // Assuming 90 degrees is the open position
  delay(9000); // Assuming 9 second to open 
  gateServo.write(0); // Return to closed position
}
