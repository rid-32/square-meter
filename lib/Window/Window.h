#include <Arduino.h>

#ifndef WIN
#define WIN

namespace win {
enum Event_Type { KEYDOWN, KEYUP, LONGKEYDOWN, SCROLL, FOCUS, UNFOCUS };

enum Scroll_Directions { FORWARD, BACKWARD };

struct Event {
  Event_Type type;
  Scroll_Directions direction;
};

class Component {
public:
  bool focused = false;
  bool should_update = true;

  void force_update();
  bool handle_focus(win::Event const *event);
  bool handle_unfocus(win::Event const *event);
  bool dispatch_event(const Event *event);
  virtual String render() = 0;
  virtual bool handle_keydown(const Event *);
  virtual bool handle_keyup(const Event *);
  virtual bool handle_longkeydown(const Event *);
  virtual bool handle_scroll(const Event *);
};

template <class Window_Page> class Window {
public:
  uint8_t pages_length;
  Window_Page **pages;

  Window(Window_Page **pages, uint8_t pages_length);
  void render();
  void dispatch_event(Event const *event);
};

template <class Page_Component> class Page {
private:
  void scroll(const Event *);
  void focusComponents();
  void renderComponents();
  void _handle_keydown(const Event *);
  void _handle_keyup(const Event *);
  void _handle_longkeydown(const Event *);
  void _handle_scroll(const Event *);

public:
  uint8_t offset_height = 0, focused_component = 0, components_length,
          viewport_height = 0;
  int8_t unfocused_component = -1;
  Page_Component **components;

  virtual bool handle_capture_keydown(const Event *);
  virtual bool handle_capture_keyup(const Event *);
  virtual bool handle_capture_longkeydown(const Event *);
  virtual bool handle_capture_scroll(const Event *);
  virtual bool handle_keydown(const Event *);
  virtual bool handle_keyup(const Event *);
  virtual bool handle_longkeydown(const Event *);
  virtual bool handle_scroll(const Event *);
  virtual void display_component(uint8_t, String);

  Page(Page_Component **, uint8_t, uint8_t);
  void render();
  void dispatch_event(Event const *);
};
} // namespace win

#endif
