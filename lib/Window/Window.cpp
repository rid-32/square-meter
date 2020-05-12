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

bool Component::handle_focus(const win::Event *event) {
  this->focused = true;

  return true;
}

bool Component::handle_unfocus(const win::Event *event) {
  this->focused = false;

  return true;
}

template <class Window_Page>
Window<Window_Page>::Window(Window_Page **pages, uint8_t pages_length) {
  this->pages = pages;
  this->pages_length = pages_length;
}

template <class Window_Page> void Window<Window_Page>::render() {
  this->pages[this->current_page]->render();
}

template <class Window_Page>
void Window<Window_Page>::dispatch_event(Event const *event) {

  this->pages[this->current_page]->dispatch_event(event);
}

template <class Page_Component>
Page<Page_Component>::Page(Page_Component **components,
                           uint8_t components_length, uint8_t viewport_height) {
  this->components = components;
  this->components_length = components_length;
  this->viewport_height = viewport_height;
}

template <class Page_Component>
bool Page<Page_Component>::handle_scroll(Event const *event) {
  bool propagation_allowed =
      this->components[this->focused_component]->dispatch_event(event);

  if (propagation_allowed) {
    bool should_update = false;

    if (event->direction == FORWARD) {
      if (this->focused_component < this->components_length - 1) {
        if (this->focused_component > this->offset_height) {
          this->offset_height++;
        }

        this->unfocused_component = this->focused_component;
        this->focused_component++;

        should_update = true;
      }
    }

    if (event->direction == BACKWARD) {
      if (this->focused_component > 0) {
        if (this->focused_component == this->offset_height) {
          this->offset_height--;
        }

        this->unfocused_component = this->focused_component;
        this->focused_component--;

        should_update = true;
      }
    }

    if (should_update) {
      this->components[this->focused_component]->should_update = true;
      this->components[this->unfocused_component]->should_update = true;

      this->render();
    }
  }

  return true;
}

template <class Page_Component> void Page<Page_Component>::focusComponents() {
  Event event;
  Page_Component *component = this->components[this->unfocused_component];

  if (component->should_update && this->unfocused_component >= 0) {
    event.type = UNFOCUS;

    component->dispatch_event(&event);
  }

  component = this->components[this->focused_component];

  if (component->should_update) {
    event.type = FOCUS;

    component->dispatch_event(&event);
  }
}

template <class Page_Component> void Page<Page_Component>::renderComponents() {
  for (uint8_t index = this->offset_height, row = 0;
       row < this->viewport_height && index < this->components_length;
       index++, row++) {
    if (this->components[index]->should_update) {
      this->components[index]->should_update = false;

      String message = this->components[index]->render();

      this->display_component(row, message);
    }
  }
}

template <class Page_Component>
void Page<Page_Component>::display_component(uint8_t row, String message) {}

template <class Page_Component> void Page<Page_Component>::render() {
  this->focusComponents();
  this->renderComponents();
}

template <class Page_Component>
void Page<Page_Component>::dispatch_event(Event const *event) {
  if (event->type == SCROLL) {
    this->handle_scroll(event);

    return;
  }

  this->components[this->focused_component]->dispatch_event(event);
}
