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

template <class Page_Component> class Page : public win::Event_Component {
protected:
  uint8_t offset_height = 0, focused_component = 0, components_length,
          viewport_height = 0;
  int8_t unfocused_component = -1;
  Page_Component *components;

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
        this->components[this->focused_component].should_update = true;
        this->components[this->unfocused_component].should_update = true;

        this->render();
      }
    }

    return true;
  }

  void focusComponents() {
    win::Event event;
    Page_Component *component = &this->components[this->unfocused_component];

    if (component->should_update && this->unfocused_component >= 0) {
      event.type = win::UNFOCUS;

      component->dispatch_event(&event);
    }

    component = &this->components[this->focused_component];

    if (component->should_update) {
      event.type = win::FOCUS;

      component->dispatch_event(&event);
    }
  }

  void renderComponents() {
    for (uint8_t index = this->offset_height, row = 0;
         row < this->viewport_height && index < this->components_length;
         index++, row++) {
      if (this->components[index].should_update) {
        this->components[index].should_update = false;

        String message = this->components[index].render();

        this->display_component(row, message);
      }
    }
  }

  virtual void display_component(uint8_t row, String message) {}

public:
  Page(Page_Component components[], uint8_t components_length,
       uint8_t viewport_height) {
    this->components = components;
    this->components_length = components_length;
    this->viewport_height = viewport_height;
  }

  void render() {
    this->focusComponents();
    this->renderComponents();
  }

  void dispatch_event(win::Event const *event) {
    if (event->type == win::SCROLL) {
      this->handle_scroll(event);

      return;
    }

    this->components[this->focused_component].dispatch_event(event);
  }

  friend class win::Window<Page>;
};

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

  friend class Page<Counter>;
};

class LCD_1602_PAGE : public Page<Counter> {
private:
  void display_component(uint8_t row, String message) {
    lcd.setCursor(0, row);
    lcd.print(message);
  }

public:
  LCD_1602_PAGE(Counter components[], uint8_t components_length,
                uint8_t viewport_height)
      : Page(components, components_length, viewport_height) {}
};

Counter rotation(16, "Оборот: ", 8);
Counter distance(16, "Длина: ", 7, "m", 1);
Counter width(16, "Ширина: ", 8, "m", 1);
Counter area(16, "Всего: ", 7, "Га", 2);
Counter components[] = {rotation, distance, width, area};

LCD_1602_PAGE settings(components, 4, 2);
LCD_1602_PAGE pages[] = {settings};

win::Window<LCD_1602_PAGE> window(pages, 1);

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
