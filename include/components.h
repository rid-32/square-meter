#include <Arduino.h>
#include <Window.h>

class LCD_1602_Component : public win::Component {
public:
  uint8_t viewport_width, head_length, tail_length, precise, curr_precise;
  uint16_t counter;
  uint16_t *counter_ref;
  double double_counter;
  const char *head, *tail;
  bool choosen;
  win::History *history;

  virtual String render() = 0;
  virtual String get_pointer();
  virtual bool handle_keyup(const win::Event *);
  virtual bool handle_scroll(const win::Event *);
};

class Counter : public LCD_1602_Component {
public:
  bool choosen = false;

  Counter(uint8_t, const char *, uint8_t, const char *, uint8_t);
  String get_pointer();
  virtual String renderCounter();
  String render();
  bool handle_longkeydown(const win::Event *);
};

class Simple_Counter : public Counter {
public:
  Simple_Counter(uint8_t viewport_width, const char *head, uint8_t head_length,
                 const char *tail = "", uint8_t tail_length = 0,
                 uint16_t initial_counter = 0)
      : Counter(viewport_width, head, head_length, tail, tail_length) {
    this->counter = initial_counter;
  }
  bool handle_keyup(const win::Event *);
  bool handle_scroll(const win::Event *);
  String renderCounter();
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

  bool handle_keyup(const win::Event *);
  bool handle_scroll(const win::Event *);
  String renderCounter();
};

class Label : public LCD_1602_Component {
public:
  Label(uint8_t, const char *, const uint8_t);
  String render();
};
