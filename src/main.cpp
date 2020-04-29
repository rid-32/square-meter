#include <Arduino.h>
#include <Ctrl.h>
#include <LiquidCrystal.h>
#include <Window.cpp>
#include <Window.h>

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

class Counter : public win::Component {
private:
  uint8_t viewport_width, head_length, tail_length;
  uint16_t counter;
  char const *head;
  char const *tail;
  bool choosen = false;

  String get_pointer() {
    String pointer = " ";

    if (this->focused) {
      pointer = String(">");
    }

    if (this->choosen) {
      pointer = String("*");
    }

    return pointer;
  }

  bool handle_keyup(win::Event const *event) {
    this->choosen = !this->choosen;
    this->should_update = true;

    return false;
  }

  bool handle_scroll(win::Event const *event) {
    if (this->choosen) {
      if (event->direction == win::FORWARD) {
        this->counter++;
      }

      if (event->direction == win::BACKWARD && this->counter > 0) {
        this->counter--;
      }

      this->should_update = true;

      return false;
    }

    return true;
  }

  String render() {
    String pointer = this->get_pointer();
    String head = pointer + String(this->head);
    String counter = String(this->counter);
    String tail = counter + String(this->tail);
    uint8_t spaces_length = this->viewport_width - pointer.length() -
                            this->head_length - counter.length() -
                            this->tail_length;
    String spaces = "";

    for (uint8_t i = 0; i < spaces_length; i++) {
      spaces.concat(String(" "));
    }

    return head + spaces + tail;
  }

public:
  Counter(uint8_t viewport_width, char const *head, uint8_t head_length,
          char const *tail = "", uint8_t tail_length = 0,
          uint16_t initial_counter = 0) {
    this->viewport_width = viewport_width;
    this->head = head;
    this->head_length = head_length;
    this->tail = tail;
    this->tail_length = tail_length;
    this->counter = initial_counter;
  }

  friend class win::Page<Counter>;
};

class LCD_1602_Page : public win::Page<Counter> {
private:
  void display_component(uint8_t row, String message) {
    lcd.setCursor(0, row);
    lcd.print(message);
  }

public:
  LCD_1602_Page(Counter components[], uint8_t components_length,
                uint8_t viewport_height)
      : Page(components, components_length, viewport_height) {}
};

Counter rotation(16, "Оборот: ", 8);
Counter distance(16, "Длина: ", 7, "m", 1);
Counter width(16, "Ширина: ", 8, "m", 1);
Counter area(16, "Всего: ", 7, "Га", 2);
Counter components[] = {rotation, distance, width, area};

LCD_1602_Page settings(components, 4, 2);
LCD_1602_Page pages[] = {settings};

win::Window<LCD_1602_Page> window(pages, 1);

void setup() {
  // Serial.begin(115200);

  lcd.begin(16, 2);

  pinMode(BUTTON, INPUT);
  pinMode(L_ENC_PIN, INPUT_PULLUP);
  pinMode(R_ENC_PIN, INPUT_PULLUP);

  enc.on("rotate", handle_rotate);
  btn.on("keyup", handle_keyup);
  btn.on("longkeydown", handle_long_keydown);
}

void loop() {
  enc.listen();
  btn.listen();

  window.render();
}

void handle_rotate(ctrl::Encoder_Event const *event) {
  win::Event window_event;

  window_event.type = win::SCROLL;

  if (event->positive_tick) {
    window_event.direction = win::FORWARD;
  }

  if (event->negative_tick) {
    window_event.direction = win::BACKWARD;
  }

  window.dispatch_event(&window_event);
}

void handle_keyup(ctrl::Button_Event const *event) {
  win::Event window_event;

  window_event.type = win::KEYUP;

  window.dispatch_event(&window_event);
}

void handle_long_keydown(ctrl::Button_Event const *event) {
  // Serial.println("Long Keydown");
}
