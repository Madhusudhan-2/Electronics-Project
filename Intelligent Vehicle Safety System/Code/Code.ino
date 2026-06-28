#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// ---------------- LED CONFIG ----------------
#define LEFT_LED_PIN 6
#define RIGHT_LED_PIN 7
#define NUM_LEDS 8

Adafruit_NeoPixel leftStrip(NUM_LEDS, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightStrip(NUM_LEDS, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);

// ---------------- SERVO + SENSOR PINS ----------------
#define LEFT_SERVO_PIN 3
#define RIGHT_SERVO_PIN 5

#define LEFT_TRIG 8
#define LEFT_ECHO 9

#define RIGHT_TRIG 11
#define RIGHT_ECHO 12

Servo leftServo;
Servo rightServo;

// ---------------- SERVO SWEEP LIMITS (UPDATED) ----------------
// Both servos move from 90° to 140° and back, in steps of 10°
int leftMin = 90;
int leftMax = 140;
int rightMin = 90;
int rightMax = 140;

int leftAngle = 90;
int rightAngle = 90;
int servoStep = 10;         // 10 degree per step
int servoDelay = 300;       // delay for each 10° step (slow movement)

// ---------------- COLOR SETTINGS ----------------
uint32_t normalColor = Adafruit_NeoPixel::Color(255, 255, 255);

// ---------------- READ DISTANCE ----------------
long readDistanceCM(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(3);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000); // 30 ms timeout
  if (duration == 0) return -1;               // no reading
  return duration * 0.0343 / 2;
}

// ---------------- DISTANCE → LED COUNT ----------------
// >=150 cm : 8 LEDs
// >=100 cm : 6 LEDs
// >= 50 cm : 4 LEDs
// <  50 cm : 2 LEDs
int ledsFromDistance(long dist) {
  if (dist < 0) return 0;      // no object

  if (dist >= 150) return 8;
  else if (dist >= 100) return 6;
  else if (dist >= 50)  return 4;
  else                  return 2;
}

// ---------------- APPLY LED COUNT ON STRIP ----------------
void setLedsByCount(Adafruit_NeoPixel &strip, int count) {
  // Turn all off first
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0); // off
  }

  // Light from LED 0 upwards
  for (int i = 0; i < count && i < NUM_LEDS; i++) {
    strip.setPixelColor(i, normalColor);
  }

  strip.show();
}

// ---------------- ANGLE CATEGORY (NEW) ----------------
// Compares left/right servo angles and returns which side is "leading"
String angleCategory(int aLeft, int aRight) {
  if (aLeft == aRight) return "CENTER";
  if (aLeft < aRight)  return "LEFT";   // left angle smaller (= more to left)
  return "RIGHT";
}

// ---------------- SETUP ----------------
void setup() {
  leftStrip.begin();
  rightStrip.begin();
  leftStrip.show();
  rightStrip.show();

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);

  leftServo.write(leftAngle);
  rightServo.write(rightAngle);

  Serial.begin(9600);
  Serial.println("System Started...");
}

// ---------------- LOOP ----------------
void loop() {

  long leftDist  = readDistanceCM(LEFT_TRIG, LEFT_ECHO);
  long rightDist = readDistanceCM(RIGHT_TRIG, RIGHT_ECHO);

  // Map distance to number of LEDs
  int leftLedCount  = ledsFromDistance(leftDist);
  int rightLedCount = ledsFromDistance(rightDist);

  // Update LED strips
  setLedsByCount(leftStrip, leftLedCount);
  setLedsByCount(rightStrip, rightLedCount);

  // ------ ANGLE CATEGORY (NEW) ------
  String angleSide = angleCategory(leftAngle, rightAngle);
  int angleDiff = abs(leftAngle - rightAngle);

  // ------ SERIAL PRINTS (DEBUG) ------
  Serial.print("LeftDist: ");
  Serial.print(leftDist);
  Serial.print(" cm  |  RightDist: ");
  Serial.print(rightDist);
  Serial.print(" cm  ||  Left LEDs: ");
  Serial.print(leftLedCount);
  Serial.print("  Right LEDs: ");
  Serial.print(rightLedCount);
  Serial.print("  || Left Angle: ");
  Serial.print(leftAngle);
  Serial.print("  Right Angle: ");
  Serial.print(rightAngle);
  Serial.print("  || AngleSide: ");
  Serial.print(angleSide);
  Serial.print("  || AngleDiff: ");
  Serial.println(angleDiff);

  // ------ SERVO SWEEP MOTION (90° to 140° SLOW) ------
  leftServo.write(leftAngle);
  rightServo.write(rightAngle);

  // Move both servos together
  leftAngle  += servoStep;
  rightAngle += servoStep;

  // Reverse direction at limits
  if (leftAngle >= leftMax || leftAngle <= leftMin) {
    servoStep = -servoStep;
  }

  // Slow movement: delay for each 10° step
  delay(servoDelay);
}
