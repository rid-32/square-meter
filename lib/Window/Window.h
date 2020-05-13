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

class Event_Component {
public:
  virtual bool handle_keydown(const Event *);
  virtual bool handle_keyup(const Event *);
  virtual bool handle_longkeydown(const Event *);
  virtual bool handle_scroll(const Event *);
  virtual bool handle_focus(const Event *);
  virtual bool handle_unfocus(const Event *);

  bool dispatch_event(const Event *event);
};

class Component : public Event_Component {
public:
  bool focused = false;
  bool should_update = true;

  virtual String render() = 0;
  void force_update();
  bool handle_focus(win::Event const *event);
  bool handle_unfocus(win::Event const *event);
};

template <class Window_Page> class Window {
public:
  uint8_t pages_length;
  Window_Page **pages;

  Window(Window_Page **pages, uint8_t pages_length);
  void render();
  void dispatch_event(Event const *event);
};

template <class Page_Component> class Page : public Event_Component {
public:
  uint8_t offset_height = 0, focused_component = 0, components_length,
          viewport_height = 0;
  int8_t unfocused_component = -1;
  Page_Component **components;

  bool handle_scroll(const Event *);
  void focusComponents();
  void renderComponents();
  virtual void display_component(uint8_t, String);

  Page(Page_Component **, uint8_t, uint8_t);
  void render();
  void dispatch_event(Event const *);
};
} // namespace win

#endif
