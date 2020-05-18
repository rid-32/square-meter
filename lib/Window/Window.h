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

class History {
private:
  uint8_t location;
  int8_t prev_location = -1;

public:
  History(uint8_t);
  void push(uint8_t);
  uint8_t get_location();
  int8_t get_prev_location();
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
private:
  void unmount_page(Window_Page *);
  void mount_page(Window_Page *);
  void render_page(const uint8_t, const int8_t = -1);

public:
  uint8_t pages_length;
  Window_Page **pages;
  History *history;

  Window(Window_Page **pages, uint8_t pages_length);
  void connect(History *);
  void render();
  void dispatch_event(const Event *);
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
  bool mounted = false;
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
  virtual void handle_did_mount();
  virtual void handle_will_unmount();

  Page(Page_Component **, uint8_t, uint8_t);
  void render();
  void dispatch_event(Event const *);
};
} // namespace win

#endif
