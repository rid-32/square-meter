#include "components.h"
#include <Utils.h>

String LCD_1602_Component::get_pointer() { return String(""); }
bool LCD_1602_Component::handle_keyup(const win::Event *) { return true; }
bool LCD_1602_Component::handle_scroll(const win::Event *) { return true; }
String LCD_1602_Component::renderCounter() { return String(""); }
void LCD_1602_Component::save_counter() { return; }

Counter::Counter(uint8_t viewport_width, const char *head, uint8_t head_length,
                 const char *tail = "", uint8_t tail_length = 0) {
  this->viewport_width = viewport_width;
  this->head = head;
  this->head_length = head_length;
  this->tail = tail;
  this->tail_length = tail_length;
}

String Counter::get_pointer() {
  String pointer = " ";

  if (this->focused) {
    pointer = String(">");
  }

  if (this->choosen) {
    pointer = String("*");
  }

  return pointer;
}

String Counter::render() {
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

bool Counter::handle_longkeydown(const win::Event *event) {
  if (this->choosen) {
    return false;
  }

  return true;
}

bool Simple_Counter::handle_keyup(const win::Event *event) {
  this->choosen = !this->choosen;
  this->should_update = true;

  if (!this->choosen) {
    this->save_counter();
  }

  return false;
}

bool Simple_Counter::handle_scroll(const win::Event *event) {
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

String Simple_Counter::renderCounter() { return String(this->counter); }

bool Precise_Counter::handle_keyup(const win::Event *event) {
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

  if (!this->choosen) {
    this->save_counter();
  }

  return false;
}

bool Precise_Counter::handle_scroll(const win::Event *event) {
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

String Precise_Counter::renderCounter() { return String(this->double_counter); }

Label::Label(uint8_t viewport_width, const char *head,
             const uint8_t head_length) {
  this->viewport_width = viewport_width;
  this->head = head;
  this->head_length = head_length;
}

String Label::render() {
  const String message = String(this->head);
  uint8_t left_spaces_length = (this->viewport_width - this->head_length) / 2;
  uint8_t right_spaces_length =
      (this->viewport_width - this->head_length - left_spaces_length);
  String left_spaces = "";
  String right_spaces = "";
  uint8_t idx;

  for (idx = 0; idx < left_spaces_length; idx++) {
    left_spaces.concat(String(" "));
  }

  for (idx = 0; idx < right_spaces_length; idx++) {
    right_spaces.concat(String(" "));
  }

  return left_spaces + message + right_spaces;
}
