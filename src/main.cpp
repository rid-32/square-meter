#include "components.h"
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
#define VIEWPORT_HEIGHT 2
#define VIEWPORT_WIDTH 16
#define HOME_PAGE 0
#define SETTINGS_PAGE 1

typedef LCD_1602_RUS<LiquidCrystal> LCD_1602;

LCD_1602 lcd(2, 3, 4, 5, 6, 7);

ctrl::Button btn(LOW, BUTTON);
ctrl::Encoder enc(0x03, L_ENC_PIN, R_ENC_PIN);

void handle_rotate(const ctrl::Encoder_Event *);
void handle_keyup(const ctrl::Button_Event *);
void handle_long_keydown(const ctrl::Button_Event *);

win::History history(HOME_PAGE);

double area_done = 12.25;

class Area_Done : public LCD_1602_Component {
public:
  Area_Done(uint8_t viewport_width) { this->viewport_width = viewport_width; }

  String render() {
    const String value = String(area_done);
    const String tail = String(" Га");
    const uint8_t value_length = value.length() + 3;

    uint8_t left_spaces_length = (this->viewport_width - value_length) / 2;
    uint8_t right_spaces_length =
        (this->viewport_width - value_length - left_spaces_length);
    String left_spaces = "";
    String right_spaces = "";
    uint8_t idx;

    for (idx = 0; idx < left_spaces_length; idx++) {
      left_spaces.concat(String(" "));
    }

    for (idx = 0; idx < right_spaces_length; idx++) {
      right_spaces.concat(String(" "));
    }

    return left_spaces + value + tail + right_spaces;
  }
};

class Reset : public LCD_1602_Component {
public:
  Reset(uint8_t viewport_width, History *history) {
    this->viewport_width = viewport_width;
    this->history = history;
  }

  String get_pointer() {
    String pointer = " ";

    if (this->focused) {
      pointer = String(">");
    }

    return pointer;
  }

  bool handle_keyup(const win::Event *event) {
    if (area_done) {
      area_done = 0.0;

      this->history->push(HOME_PAGE);
    }

    return false;
  }

  String render() {
    const String pointer = this->get_pointer();
    const String message = "Сброс";
    String spaces;

    for (uint8_t i = 0; i < message.length() + 1; i++) {
      spaces += " ";
    }

    return pointer + message + spaces;
  }
};

template <class LCD_Component>
class LCD_1602_Page : public win::Page<LCD_1602_Component> {
public:
  bool is_mounted_after_navigation;
  History *history;
  LCD_Component *lcd;

  LCD_1602_Page(LCD_1602_Component **components, uint8_t components_length,
                uint8_t viewport_height, LCD_Component *lcd)
      : Page(components, components_length, viewport_height) {
    this->lcd = lcd;
  }

  void display_component(uint8_t row, String message) {
    this->lcd->setCursor(0, row);
    this->lcd->print(message);
  }
};

class Home_Page : public LCD_1602_Page<LCD_1602> {
public:
  Home_Page(LCD_1602_Component **components, uint8_t components_length,
            uint8_t viewport_height, History *history, LCD_1602 *lcd)
      : LCD_1602_Page(components, components_length, viewport_height, lcd) {
    this->history = history;
  };

  bool handle_capture_longkeydown(const win::Event *event) {
    this->history->push(SETTINGS_PAGE);

    return false;
  }
};

class Settings_Page : public LCD_1602_Page<LCD_1602> {
public:
  Settings_Page(LCD_1602_Component **components, uint8_t components_length,
                uint8_t viewport_height, History *history, LCD_1602 *lcd)
      : LCD_1602_Page(components, components_length, viewport_height, lcd) {
    this->history = history;
  };

  void handle_did_mount() {
    const int8_t prev_location = this->history->get_prev_location();

    if (~prev_location) {
      this->is_mounted_after_navigation = true;
    } else {
      this->is_mounted_after_navigation = false;
    }
  }

  bool handle_longkeydown(const win::Event *event) {
    this->history->push(HOME_PAGE);

    return false;
  }

  bool handle_capture_keyup(const win::Event *event) {
    if (this->is_mounted_after_navigation) {
      this->is_mounted_after_navigation = false;

      return false;
    }

    return true;
  }

  bool handle_capture_scroll(const win::Event *event) {
    if (this->is_mounted_after_navigation) {
      this->is_mounted_after_navigation = false;

      return false;
    }

    return true;
  }
};

Simple_Counter rotation(VIEWPORT_WIDTH, "Оборот: ", 8);
Simple_Counter distance(VIEWPORT_WIDTH, "Длина: ", 7, "m", 1);
Precise_Counter width(VIEWPORT_WIDTH, "Ширина: ", 8, "m", 1, 2);

Label done_label(VIEWPORT_WIDTH, "Обработано:", 11);
Area_Done area_done_label(VIEWPORT_WIDTH);

Reset reset(VIEWPORT_WIDTH, &history);

LCD_1602_Component *settings_components[] = {&rotation, &distance, &width,
                                             &reset};
LCD_1602_Component *home_components[] = {&done_label, &area_done_label};

Settings_Page settings(settings_components, 4, VIEWPORT_HEIGHT, &history, &lcd);
Home_Page home(home_components, 2, VIEWPORT_HEIGHT, &history, &lcd);
LCD_1602_Page<LCD_1602> *pages[] = {&home, &settings};

win::Window<LCD_1602_Page<LCD_1602>> window(pages, 2);

void setup() {
  // Serial.begin(115200);

  lcd.begin(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  pinMode(BUTTON, INPUT);
  pinMode(L_ENC_PIN, INPUT_PULLUP);
  pinMode(R_ENC_PIN, INPUT_PULLUP);

  enc.on("rotate", handle_rotate);
  btn.on("keyup", handle_keyup);
  btn.on("longkeydown", handle_long_keydown);

  window.connect(&history);
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
