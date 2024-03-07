#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"
#include "GyverButton.h"
#include <GyverNTC.h>

GyverNTC therm(0, 10000, 6500);

#define CLK 12
#define DT 11
#define SW 10
Encoder enc1(CLK, DT, SW, TYPE1);  // для работы c кнопкой и сразу выбираем тип

#define START_BUTTON_PIN 9
#define Temp_BUTTON_PIN 6
GButton butt1(START_BUTTON_PIN);
GButton butt2(Temp_BUTTON_PIN);
#define Vozduh 7
#define Svarka 8

volatile int time1 = 0;
volatile int time2 = 0;

volatile int butt1Pressed = 0;

unsigned long startTime = 0;  // Переменная для хранения времени начала задержки

int prevTime1 = 0;
int prevTime2 = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Устанавливаем дисплей

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("STime:");
  lcd.print(time1);
  lcd.setCursor(0, 1);
  lcd.print("Time:");
  lcd.print(time2);
  butt1.setTickMode(AUTO);
  butt2.setTickMode(AUTO);
  pinMode(Svarka, OUTPUT);
  pinMode(Vozduh, OUTPUT);
  digitalWrite(Svarka, LOW);
  digitalWrite(Vozduh, LOW);
}


void loop() {
  startButtonPressed();
  startButtonSwitch();
  WarnButton();
  isrEncoder1();
  if (time1 != prevTime1 || time2 != prevTime2) {
    updateDisplay();
    prevTime1 = time1;
    prevTime2 = time2;
  }
}

void isrEncoder1() {
  enc1.tick();
  if (enc1.isLeft()) {
    time2 = max(0, time2 - 100);
  }
  if (enc1.isRight()) {
    time2 = min(3000, time2 + 100);
  }
  if (enc1.isLeftH()) {
    time1 = max(0, time1 - 100);
  }
  if (enc1.isRightH()) {
    time1 = min(3000, time1 + 100);
  }
}

void startButtonPressed() {
  butt1.tick();
  if (butt1.isPress() && butt2.isHold() && therm.getTempAverage() <= 65 && time2 >= 100) {
    startTime = millis();
    if (butt1Pressed == 1 && time1 >= 100) {
      butt1Pressed = 0;
    } else if (time1 >= 100) {
      butt1Pressed = 1;
    } else {
      butt1Pressed = 2;
    }
  if (butt1.isHold() && butt2.isHold() && therm.getTempAverage() <= 65 && time2 >= 100){
    butt1Pressed = 3;

  }

  }
}
void startButtonSwitch() {
  switch (butt1Pressed) {
    case 0:
      digitalWrite(Vozduh, LOW);
      digitalWrite(Svarka, LOW);
      break;
    case 1:
      digitalWrite(Vozduh, HIGH);
      if (millis() - startTime >= 200) {
        digitalWrite(Svarka, HIGH);
      }
      if (millis() - startTime >= (time2 + 500)) {
        digitalWrite(Vozduh, LOW);
      }
      if (millis() - startTime >= time2 + 200) {
        digitalWrite(Svarka, LOW);
      }
      break;
    case 2:
      digitalWrite(Vozduh, HIGH);
      if (millis() - startTime >= 200) {
        digitalWrite(Svarka, HIGH);
      }
      if (millis() - startTime >= (time2 + 500)) {
        digitalWrite(Vozduh, LOW);
      }
      if (millis() - startTime >= time2 + 200) {
        digitalWrite(Svarka, LOW);
        butt1Pressed = 0;
      }
      break;
  }
}


void WarnButton() {
  butt2.tick();
  lcd.setCursor(13, 0);
  lcd.print(therm.getTempAverage());
  lcd.setCursor(15, 0);
  lcd.print("C");
  if (!butt2.isHold()) {
    lcd.setCursor(9, 1);
    lcd.print("WarnTemp");
  } else {
    lcd.setCursor(9, 1);
    lcd.print("TEMP_OK");
  }
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STime:");
  lcd.print(time1);
  lcd.setCursor(0, 1);
  lcd.print("Time:");
  lcd.print(time2);
}
