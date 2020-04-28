#include "Window.h"

using namespace win;

bool Event_Component::handle_keydown(win::Event const *) { return true; }

bool Event_Component::handle_keyup(win::Event const *) { return true; }

bool Event_Component::handle_longkeydown(win::Event const *) { return true; }

bool Event_Component::handle_scroll(win::Event const *) { return true; }

bool Event_Component::handle_focus(win::Event const *) { return true; }

bool Event_Component::handle_unfocus(win::Event const *) { return true; }

bool Event_Component::dispatch_event(win::Event const *event) {
  if (event->type == win::KEYDOWN) {
    return this->handle_keydown(event);
  }

  if (event->type == win::KEYUP) {
    return this->handle_keyup(event);
  }

  if (event->type == win::LONGKEYDOWN) {
    return this->handle_longkeydown(event);
  }

  if (event->type == win::SCROLL) {
    return this->handle_scroll(event);
  }

  if (event->type == win::FOCUS) {
    return this->handle_focus(event);
  }

  if (event->type == win::UNFOCUS) {
    return this->handle_unfocus(event);
  }

  return true;
}

bool Component::handle_focus(win::Event const *event) {
  this->focused = true;

  return true;
}

bool Component::handle_unfocus(win::Event const *event) {
  this->focused = false;

  return true;
}

String Component::render() { return String(""); };

template <class Window_Page>
Window<Window_Page>::Window(Window_Page pages[], uint8_t pages_length) {
  this->pages = pages;
  this->pages_length = pages_length;
}

template <class Window_Page> void Window<Window_Page>::render() {
  this->pages[this->current_page].render();
}

template <class Window_Page>
void Window<Window_Page>::dispatch_event(Event const *event) {

  this->pages[this->current_page].dispatch_event(event);
}

// template <class Page_Component>
// Page<Page_Component>::Page(Page_Component components[],
//                            uint8_t components_length, uint8_t
//                            viewport_height) {
//   this->components = components;
//   this->components_length = components_length;
//   this->viewport_height = viewport_height;
// }
//
// template <class Page_Component>
// bool Page<Page_Component>::handle_scroll(win::Event const *event) {
//   bool propagation_allowed =
//       this->components[this->focused_component].dispatch_event(event);
//
//   if (propagation_allowed) {
//     bool should_update = false;
//
//     if (event->direction == win::FORWARD) {
//       if (this->focused_component < this->components_length - 1) {
//         if (this->focused_component > this->offset_height) {
//           this->offset_height++;
//         }
//
//         this->unfocused_component = this->focused_component;
//         this->focused_component++;
//
//         should_update = true;
//       }
//     }
//
//     if (event->direction == win::BACKWARD) {
//       if (this->focused_component > 0) {
//         if (this->focused_component == this->offset_height) {
//           this->offset_height--;
//         }
//
//         this->unfocused_component = this->focused_component;
//         this->focused_component--;
//
//         should_update = true;
//       }
//     }
//
//     if (should_update) {
//       this->render();
//     }
//   }
//
//   return true;
// }
//
// template <class Page_Component>
// void Page<Page_Component>::dispatch_event(win::Event const *event) {
//   if (event->type == win::SCROLL) {
//     this->handle_scroll(event);
//
//     return;
//   }
//
//   this->components[this->focused_component].dispatch_event(event);
// }
//
// template <class Page_Component> void Page<Page_Component>::focusComponents()
// {
//   win::Event event;
//
//   if (this->unfocused_component >= 0) {
//     event.type = win::UNFOCUS;
//
//     this->components[this->unfocused_component].dispatch_event(&event);
//   }
//
//   event.type = win::FOCUS;
//
//   this->components[this->focused_component].dispatch_event(&event);
// }
//
// template <class Page_Component> void Page<Page_Component>::renderComponents()
// {
//   for (uint8_t i = this->offset_height, j = 0;
//        j < this->viewport_height && i < this->components_length; i++, j++) {
//     // lcd.setCursor(0, j);
//
//     this->components[i].render();
//   }
// }
//
// template <class Page_Component> void Page<Page_Component>::render() {
//   this->focusComponents();
//
//   this->renderComponents();
// }
