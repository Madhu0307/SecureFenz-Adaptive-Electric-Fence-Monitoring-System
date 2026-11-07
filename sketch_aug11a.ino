#define BLYNK_TEMPLATE_ID "TMPL3tEwkFWL3"
#define BLYNK_TEMPLATE_NAME "Smart Fence"
#define BLYNK_AUTH_TOKEN "K7oxsjMGKQiLhR722bCDDrBUqEXwA-d1"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "ACT-ai_Muregesan";
char pass[] = "Clu3@T0p";

LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

#define PIR_PIN 19
#define VIBRATION_PIN 27
#define RAIN_PIN 33

#define TRIG_PIN 5
#define ECHO_PIN 18

#define GREEN_LED 25
#define YELLOW_LED 26
#define RED_LED 14
#define BUZZER 13
bool previousMotion = LOW;
bool previousVibration = LOW;

float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2;
}

void sendToBlynk() {

  int motion = digitalRead(PIR_PIN);
  int vibration = digitalRead(VIBRATION_PIN);
  int rain = digitalRead(RAIN_PIN);

  float distance = getDistance();

  if (motion == HIGH) {
    Blynk.virtualWrite(V0, 1);
  }
  else {
    Blynk.virtualWrite(V0, 0);
  }

  if (vibration == HIGH) {
    Blynk.virtualWrite(V1, 1);
  }
  else {
    Blynk.virtualWrite(V1, 0);
  }
  if (rain == LOW) {
    Blynk.virtualWrite(V2, 1);
  }
  else {
    Blynk.virtualWrite(V2, 0);
  }

  Blynk.virtualWrite(V3, distance);

  if (rain == LOW) {

    Blynk.virtualWrite(V4, 1);

  }
  else if (vibration == HIGH) {

    Blynk.virtualWrite(V4, 2);

  }
  else if (motion == HIGH) {

    Blynk.virtualWrite(V4, 3);

  }
  else if (distance < 50) {

    Blynk.virtualWrite(V4, 4);

  }
  else {

    Blynk.virtualWrite(V4, 0);

  }

  if (motion == HIGH && previousMotion == LOW) {

    Blynk.logEvent(
      "motion",
      "Motion detected near the fence!"
    );

    Serial.println("BLYNK MOTION EVENT SENT");
  }

  previousMotion = motion;

  if (vibration == HIGH && previousVibration == LOW) {

    Blynk.logEvent(
      "tampering",
      "Fence tampering detected!"
    );

    Serial.println("BLYNK TAMPERING EVENT SENT");
  }

  previousVibration = vibration;

  Serial.print("Motion: ");
  Serial.print(motion);

  Serial.print(" | Vibration: ");
  Serial.print(vibration);

  Serial.print(" | Rain: ");
  Serial.print(rain);

  Serial.print(" | Distance: ");
  Serial.println(distance);
}

void setup() {

  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SMART FENCE");

  lcd.setCursor(0, 1);
  lcd.print("CONNECTING...");

  // Blynk + WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SYSTEM READY");

  delay(1500);

  lcd.clear();

  // Run sendToBlynk every 1 second
  timer.setInterval(1000L, sendToBlynk);
}

void loop() {

  Blynk.run();

  timer.run();

  int motion = digitalRead(PIR_PIN);
  int vibration = digitalRead(VIBRATION_PIN);
  int rain = digitalRead(RAIN_PIN);

  float distance = getDistance();
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
  if (rain == LOW) {

    digitalWrite(YELLOW_LED, HIGH);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("RAIN DETECTED");

    Serial.println("RAIN DETECTED");
  }


  else if (vibration == HIGH) {

    digitalWrite(RED_LED, HIGH);

    // Continuous buzzer
    digitalWrite(BUZZER, HIGH);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("TAMPERING");

    lcd.setCursor(0, 1);
    lcd.print("DETECTED");

    Serial.println("TAMPERING DETECTED");
  }

  else if (motion == HIGH) {

    digitalWrite(YELLOW_LED, HIGH);

    digitalWrite(BUZZER, HIGH);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("MOTION");

    lcd.setCursor(0, 1);
    lcd.print("DETECTED");

    Serial.println("MOTION DETECTED");
  }
  else if (distance < 50) {

    digitalWrite(GREEN_LED, HIGH);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("OBJECT NEAR");

    lcd.setCursor(0, 1);
    lcd.print("Dist:");
    lcd.print(distance, 1);
    lcd.print("cm");

    Serial.print("OBJECT NEAR: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  else {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SAFE");

    Serial.println("SAFE");
  }

  delay(100);
}