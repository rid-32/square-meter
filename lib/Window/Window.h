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
protected:
  virtual bool handle_keydown(Event const *);
  virtual bool handle_keyup(Event const *);
  virtual bool handle_longkeydown(Event const *);
  virtual bool handle_scroll(Event const *);
  virtual bool handle_focus(Event const *);
  virtual bool handle_unfocus(Event const *);

  bool dispatch_event(Event const *event);
};

class Component : public Event_Component {
protected:
  bool focused = false;
  bool should_update = true;

  bool handle_focus(win::Event const *event);
  bool handle_unfocus(win::Event const *event);
  virtual String render();
};

template <class Window_Page> class Window {
private:
  uint8_t current_page = 0, pages_length;
  int8_t prev_page = -1;
  Window_Page *pages;

public:
  Window(Window_Page pages[], uint8_t pages_length);
  void render();
  void dispatch_event(Event const *event);
};

template <class Page_Component> class Page : public Event_Component {
protected:
  uint8_t offset_height = 0, focused_component = 0, components_length,
          viewport_height = 0;
  int8_t unfocused_component = -1;
  Page_Component *components;

  bool handle_scroll(Event const *);
  void focusComponents();
  void renderComponents();
  virtual void display_component(uint8_t, String);

public:
  Page(Page_Component *, uint8_t, uint8_t);
  void render();
  void dispatch_event(Event const *);

  friend class Window<Page>;
};
} // namespace win

#endif
