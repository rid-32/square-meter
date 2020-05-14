#include <Arduino.h>
#include <Ctrl.h>
#include <LiquidCrystal.h>
#include <Utils.h>
#include <Window.cpp>
#include <Window.h>

#define _LCD_TYPE 2
#include "LCD_1602_RUS_ALL.h"

#define BUTTON 8
#define L_ENC_PIN 9
#define R_ENC_PIN 10
#define VIEWPORT_HEIGHT 2
#define VIEWPORT_WIDTH 16

LCD_1602_RUS<LiquidCrystal> lcd(2, 3, 4, 5, 6, 7);

ctrl::Button btn(LOW, BUTTON);
ctrl::Encoder enc(0x03, L_ENC_PIN, R_ENC_PIN);

void handle_rotate(const ctrl::Encoder_Event *);
void handle_keyup(const ctrl::Button_Event *);
void handle_long_keydown(const ctrl::Button_Event *);

class LCD_1602_Component : public win::Component {
public:
  uint8_t viewport_width, head_length, tail_length, precise, curr_precise;
  uint16_t counter;
  uint16_t *counter_ref;
  double double_counter;
  const char *head, *tail, *message;
  bool choosen;

  virtual String render() = 0;
  virtual String get_pointer() { return String(""); };
  virtual bool handle_keyup(const win::Event *) { return true; };
  virtual bool handle_scroll(const win::Event *) { return true; };
};

class Counter : public LCD_1602_Component {
public:
  bool choosen = false;

  Counter(uint8_t viewport_width, char const *head, uint8_t head_length,
          char const *tail = "", uint8_t tail_length = 0) {
    this->viewport_width = viewport_width;
    this->head = head;
    this->head_length = head_length;
    this->tail = tail;
    this->tail_length = tail_length;
  }

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

  virtual String renderCounter() { return String(""); }

  String render() {
    String pointer = this->get_pointer();
    String counter = this->renderCounter();
    String head = pointer + String(this->head);
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
};

class Simple_Counter : public Counter {
public:
  Simple_Counter(uint8_t viewport_width, char const *head, uint8_t head_length,
                 char const *tail = "", uint8_t tail_length = 0,
                 uint16_t initial_counter = 0)
      : Counter(viewport_width, head, head_length, tail, tail_length) {
    this->counter = initial_counter;
  }

  bool handle_keyup(const win::Event *event) {
    this->choosen = !this->choosen;
    this->should_update = true;

    return false;
  }

  bool handle_scroll(const win::Event *event) {
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

  String renderCounter() { return String(this->counter); }
};

class Precise_Counter : public Counter {
public:
  Precise_Counter(uint8_t viewport_width, char const *head, uint8_t head_length,
                  char const *tail = "", uint8_t tail_length = 0,
                  uint8_t precise = 0, double initial_counter = 0.0)
      : Counter(viewport_width, head, head_length, tail, tail_length) {
    this->double_counter = initial_counter;
    this->precise = precise;
    this->curr_precise = 0;
  }

  bool handle_keyup(const win::Event *event) {
    if (this->choosen) {
      if (this->curr_precise < this->precise) {
        this->curr_precise++;
      } else {
        this->choosen = false;
      }
    } else {
      this->choosen = true;
      this->curr_precise = 0;
    }

    this->should_update = true;

    return false;
  }

  bool handle_scroll(const win::Event *event) {
    if (this->choosen) {
      const uint16_t divider = utils::pow(10, this->curr_precise);
      double value = 1.0 / (float)divider;

      if (event->direction == win::FORWARD) {
        this->double_counter += value;
      }

      if (event->direction == win::BACKWARD) {
        value = this->double_counter - value;

        if (value >= 0.0) {
          this->double_counter = value;
        } else {
          this->double_counter = 0.0;
        }
      }

      this->should_update = true;

      return false;
    }

    return true;
  }

  String renderCounter() { return String(this->double_counter); }
};

class Label : public LCD_1602_Component {
public:
  Label(uint8_t viewport_width, const char *message) {
    this->viewport_width = viewport_width;
    this->message = message;
  }

  String render() { return String(this->message); }
};

class Reset : public LCD_1602_Component {
public:
  Reset(uint8_t viewport_width) { this->viewport_width = viewport_width; }

  String get_pointer() {
    String pointer = " ";

    if (this->focused) {
      pointer = String(">");
    }

    return pointer;
  }

  String render() {
    String pointer = this->get_pointer();
    String message = String("Сбросить");
    String spaces;

    for (uint8_t i = 0; i < message.length() + 1; i++) {
      spaces += " ";
    }

    return pointer + message + spaces;
  }
};

class LCD_1602_Page : public win::Page<LCD_1602_Component> {
public:
  LCD_1602_Page(LCD_1602_Component **components, uint8_t components_length,
                uint8_t viewport_height)
      : Page(components, components_length, viewport_height) {}

  void display_component(uint8_t row, String message) {
    lcd.setCursor(0, row);
    lcd.print(message);
  }
};

Simple_Counter rotation(VIEWPORT_WIDTH, "Оборот: ", 8);
Simple_Counter distance(VIEWPORT_WIDTH, "Длина: ", 7, "m", 1);
Precise_Counter width(VIEWPORT_WIDTH, "Ширина: ", 8, "m", 1, 2);
Precise_Counter area(VIEWPORT_WIDTH, "Всего: ", 7, "Га", 2, 2);

Label done_label(VIEWPORT_WIDTH, "Обработано:");
Label area_label(VIEWPORT_WIDTH, "1,25 Га");

Reset reset(VIEWPORT_WIDTH);

LCD_1602_Component *settings_components[] = {&rotation, &distance, &width,
                                             &area, &reset};
LCD_1602_Component *home_components[] = {&done_label, &area_label};

LCD_1602_Page settings(settings_components, 5, VIEWPORT_HEIGHT);
LCD_1602_Page home(home_components, 2, VIEWPORT_HEIGHT);
LCD_1602_Page *pages[] = {&settings, &home};

win::Window<LCD_1602_Page> window(pages, 2);

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
  win::Event window_event;

  window_event.type = win::LONGKEYDOWN;

  window.dispatch_event(&window_event);
}
