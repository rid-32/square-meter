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
