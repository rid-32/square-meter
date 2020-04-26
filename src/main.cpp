#include <Arduino.h>
#include <Ctrl.h>
#include <LiquidCrystal.h>
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

class Component : public win::Event_Component {
protected:
  bool focused = false;

  bool handle_focus(win::Event const *event) {
    this->focused = true;

    return true;
  }

  bool handle_unfocus(win::Event const *event) {
    this->focused = false;

    return true;
  }

  virtual void render(){};

public:
  char const *greeting = "hello, world";

  friend class Page;
};

class Counter : public Component {
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
    this->render();

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

      this->render();

      return false;
    }

    return true;
  }

  void render() {
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

    String message = head + spaces + tail;

    lcd.print(message);
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

  friend class Page;
};

class Page : public win::Event_Component {
private:
  uint8_t offset_height = 0, focused_component = 0, components_length,
          viewport_height = 0;
  int8_t unfocused_component = -1;
  Counter *components;

  bool handle_scroll(win::Event const *event) {
    bool propagation_allowed =
        this->components[this->focused_component].dispatch_event(event);

    if (propagation_allowed) {
      bool should_update = false;

      if (event->direction == win::FORWARD) {
        if (this->focused_component < this->components_length - 1) {
          if (this->focused_component > this->offset_height) {
            this->offset_height++;
          }

          this->unfocused_component = this->focused_component;
          this->focused_component++;

          should_update = true;
        }
      }

      if (event->direction == win::BACKWARD) {
        if (this->focused_component > 0) {
          if (this->focused_component == this->offset_height) {
            this->offset_height--;
          }

          this->unfocused_component = this->focused_component;
          this->focused_component--;

          should_update = true;
        }
      }

      if (should_update) {
        this->render();
      }
    }

    return true;
  }

  void dispatch_event(win::Event const *event) {
    if (event->type == win::SCROLL) {
      this->handle_scroll(event);

      return;
    }

    this->components[this->focused_component].dispatch_event(event);
  }

  void focusComponents() {
    win::Event event;

    if (this->unfocused_component >= 0) {
      event.type = win::UNFOCUS;

      this->components[this->unfocused_component].dispatch_event(&event);
    }

    event.type = win::FOCUS;

    this->components[this->focused_component].dispatch_event(&event);
  }

  void renderComponents() {
    for (uint8_t i = this->offset_height, j = 0;
         j < this->viewport_height && i < this->components_length; i++, j++) {
      lcd.setCursor(0, j);

      this->components[i].render();
    }
  }

  void render() {
    this->focusComponents();

    this->renderComponents();
  }

public:
  Page(Counter components[], uint8_t components_length,
       uint8_t viewport_height) {
    this->components = components;
    this->components_length = components_length;
    this->viewport_height = viewport_height;
  }

  friend class Window;
};

class Window {
private:
  uint8_t current_page = 0, pages_length;
  int8_t prev_page = -1;
  Page *pages;

public:
  Window(Page pages[], uint8_t pages_length) {
    this->pages = pages;
    this->pages_length = pages_length;
  }

  void render() { this->pages[this->current_page].render(); };

  void dispatch_event(win::Event const *event) {
    this->pages[this->current_page].dispatch_event(event);
  };
};

Counter rotation(16, "Обороты: ", 9);
Counter distance(16, "Длина: ", 7, "м", 1);
Counter width(16, "Ширина: ", 8, "м", 1);
Counter area(16, "Всего: ", 7, "Га", 2);
Counter components[] = {rotation, distance, width, area};

Page settings(components, 4, 2);
Page pages[] = {settings};

Window window(pages, 1);

void setup() {
  // Serial.begin(115200);

  pinMode(BUTTON, INPUT);
  pinMode(L_ENC_PIN, INPUT_PULLUP);
  pinMode(R_ENC_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);

  enc.on("rotate", handle_rotate);
  btn.on("keyup", handle_keyup);
  btn.on("longkeydown", handle_long_keydown);

  window.render();
}

void loop() {
  enc.listen();
  btn.listen();
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
