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
  virtual bool handle_keydown(win::Event const *);
  virtual bool handle_keyup(win::Event const *);
  virtual bool handle_longkeydown(win::Event const *);
  virtual bool handle_scroll(win::Event const *);
  virtual bool handle_focus(win::Event const *);
  virtual bool handle_unfocus(win::Event const *);

  bool dispatch_event(win::Event const *event);
};
} // namespace win

#endif
