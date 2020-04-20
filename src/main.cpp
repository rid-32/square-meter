#include <Arduino.h>
#include <Ctrl.h>
#include <LiquidCrystal.h>

#define _LCD_TYPE 2
#include "LCD_1602_RUS_ALL.h"

#define BUTTON 8
#define L_ENC_PIN 9
#define R_ENC_PIN 10

LCD_1602_RUS<LiquidCrystal> lcd(2, 3, 4, 5, 6, 7);

ctrl::Button btn(LOW, BUTTON);
ctrl::Encoder enc(0x03, L_ENC_PIN, R_ENC_PIN);

void handle_rotate(ctrl::Encoder_Event const *);
void handle_keyup(ctrl::Button_Event const *);
void handle_long_keydown(ctrl::Button_Event const *);

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON, INPUT);
  pinMode(L_ENC_PIN, INPUT_PULLUP);
  pinMode(R_ENC_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);

  const char *progress = "Cделано площади:";
  const char *pause = "Остановлено";
  const char *value = "1,25 Га";

  lcd.print(progress);
  // lcd.print(pause);

  lcd.setCursor(0, 1);
  // при остановке количество гектаров должно моргать
  lcd.print(value);

  enc.on("rotate", handle_rotate);
  btn.on("keyup", handle_keyup);
  btn.on("longkeydown", handle_long_keydown);
}

void loop() {
  enc.listen();
  btn.listen();
}

void handle_rotate(ctrl::Encoder_Event const *event) {
  static uint8_t counter = 0;

  if (event->positive_tick) {
    Serial.println("Counter: " + String(++counter));
  }

  if (event->negative_tick && counter > 0) {
    Serial.println("Counter: " + String(--counter));
  }
}

void handle_keyup(ctrl::Button_Event const *event) {
  static uint8_t counter = 0;

  Serial.println("Button: " + String(++counter));
}

void handle_long_keydown(ctrl::Button_Event const *event) {
  Serial.println("Long Keydown");
}
