#include <ESP32Servo.h>

Servo myServo;

const int buttonPin = 2;
const int ledPin = 5;
const int servoPin = 4;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

bool isRunning = false;
bool lastButtonState = HIGH;
bool buttonPressed = false;

int pos = 0;
bool increasing = true;
unsigned long prevTime = 0;
const int moveDelay = 50;

bool waiting = false;
unsigned long waitStartTime = 0;
const unsigned long waitDuration = 60000; // 1 min


void setup() {
  Serial.begin(115200);
  delay(500);  // boot time delay
  Serial.println("Программа стартовала");
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  myServo.setPeriodHertz(50); // set PWM 50Hz for ESP32
  myServo.attach(servoPin, 500, 2400); // servo
  
  myServo.write(0); // start point
  digitalWrite(ledPin, LOW);
}

void loop() {
  bool reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == HIGH && !buttonPressed) {
      isRunning = !isRunning;
      buttonPressed = true;

      if (isRunning) {
        waiting = false;
      }
    }
    if (reading == LOW) {
      buttonPressed = false;
    }
  }

  lastButtonState = reading;

  if (isRunning) {
    digitalWrite(ledPin, HIGH);

    if (!waiting) {
      if (millis() - prevTime >= moveDelay) {
        prevTime = millis();

        if (increasing) {
          pos++;
          if (pos >= 60) {
            increasing = false;
          }
        } else {
          pos--;
          if (pos <= 0) {
            increasing = true;
            waiting = true;
            waitStartTime = millis();
          }
        }

        myServo.write(pos);
      }
    } else {
      if (millis() - waitStartTime >= waitDuration) {
        waiting = false;
      }
    }
  } else {
    digitalWrite(ledPin, LOW);

    if (pos > 0) {
      if (millis() - prevTime >= moveDelay) {
        prevTime = millis();
        pos--;
        myServo.write(pos);
      }
    }
    waiting = false;
  }
}
