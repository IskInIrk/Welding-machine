#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"
#include "GyverButton.h"
#include <GyverNTC.h>

GyverNTC therm(0, 10000, 5000);

#define CLK 12
#define DT 11
#define SW 10
Encoder enc1(CLK, DT, SW, TYPE1);  // для работы c кнопкой и сразу выбираем тип

#define START_BUTTON_PIN 9
#define Temp_BUTTON_PIN 6
GButton butt1(START_BUTTON_PIN);
GButton butt2(Temp_BUTTON_PIN);
#define Vozduh 8
#define Svarka 7

volatile int time1 = 0;
volatile int time2 = 0;

bool butt1Pressed = false;

unsigned long startTime = 0;  // Переменная для хранения времени начала задержки

int prevTime1 = 0;
int prevTime2 = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Устанавливаем дисплей

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();  // Включаем подсветку дисплея
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
  WarnButton();
  isrEncoder1();  // Обработка изменений энкодера
  if (time1 != prevTime1 || time2 != prevTime2) {
    updateDisplay();  // Обновление экрана, если значения времени изменились
    prevTime1 = time1;
    prevTime2 = time2;
  }
}

void isrEncoder1() {
  enc1.tick();
  if (enc1.isLeft()) {
    if (time2 >= 100) {
      time2 -= 100;
    } else {
      time2 = 0;
    }
  }
  if (enc1.isRight()) {
    if (time2 <= 2900) {
      time2 += 100;
    } else {
      time2 = 3000;
    }
  }

  if (enc1.isLeftH()) {
    if (time1 >= 100) {
      time1 -= 100;
    } else {
      time1 = 0;
    }
  }
  if (enc1.isRightH()) {
    if (time1 <= 2900) {
      time1 += 100;
    } else {
      time1 = 3000;
    }
  }
}

void startButtonPressed() {
  butt1.tick();
  if (butt1.isPress() && butt2.isHold() == true && therm.getTempAverage() <= 65 && time2 >= 100) {
    digitalWrite(Vozduh, HIGH);
    startTime = millis();
  }
  if (millis() - startTime >= 200) {
    digitalWrite(Svarka, HIGH);
  }

  if (millis() - startTime >= (time2 + time1 + 300)) {
    digitalWrite(Vozduh, LOW);
  }

  if (millis() - startTime >= time2 + 100) {
    digitalWrite(Svarka, LOW);
  }
}


void WarnButton() {
  butt2.tick();
  lcd.setCursor(10, 0);
  lcd.print(therm.getTempAverage());
  lcd.setCursor(15, 0);
  lcd.print("C");
  if (!butt2.isHold()) {
    lcd.setCursor(10, 1);
    lcd.print("WarnTemp");
  } else {
    lcd.setCursor(10, 1);
    lcd.print("TEMPOK");
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
