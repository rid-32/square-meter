#include <Arduino.h>
#include <LiquidCrystal.h>

#define _LCD_TYPE 2
#include "LCD_1602_RUS_ALL.h"

LCD_1602_RUS<LiquidCrystal> lcd(2, 3, 4, 5, 6, 7);

void setup() {
  lcd.begin(16, 2);

  const char *progress = "Cделано площади:";
  const char *pause = "Остановлено";
  // const double num = 1.00;
  const char *value = "1,25 Га";

  lcd.print(progress);
  // lcd.print(pause);

  lcd.setCursor(9, 1);
  // при остановке количество гектаров должно моргать
  lcd.print(value);
}

void loop() {}
