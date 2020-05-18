#include "Window.h"

using namespace win;

bool Component::handle_keydown(const win::Event *) { return true; }

bool Component::handle_keyup(const win::Event *) { return true; }

bool Component::handle_longkeydown(const win::Event *) { return true; }

bool Component::handle_scroll(const win::Event *) { return true; }

bool Component::dispatch_event(const win::Event *event) {
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

void Component::force_update() { this->should_update = true; }

template <class Window_Page>
Window<Window_Page>::Window(Window_Page **pages, uint8_t pages_length) {
  this->pages = pages;
  this->pages_length = pages_length;
}

template <class Window_Page>
void Window<Window_Page>::connect(History *history) {
  this->history = history;
}

template <class Window_Page>
void Window<Window_Page>::mount_page(Window_Page *page) {
  page->mounted = true;

  for (uint8_t i = 0; i < page->components_length; i++) {
    page->components[i]->should_update = true;
  }
}

template <class Window_Page>
void Window<Window_Page>::unmount_page(Window_Page *page) {
  page->mounted = false;
  page->offset_height = 0;
  page->focused_component = 0;
}

template <class Window_Page>
void Window<Window_Page>::render_page(const uint8_t location,
                                      const int8_t prev_location) {
  if (~prev_location) {
    Window_Page *prev_page = this->pages[prev_location];

    if (prev_page->mounted) {
      prev_page->handle_will_unmount();

      this->unmount_page(prev_page);
    }
  }

  Window_Page *page = this->pages[location];
  bool is_mounted = page->mounted;

  if (!is_mounted) {
    this->mount_page(page);
  }

  page->render();

  if (!is_mounted) {
    page->handle_did_mount();
  }
}

template <class Window_Page> void Window<Window_Page>::render() {
  if (!this->history) {
    this->render_page(0);
  }

  const uint8_t location = this->history->get_location();
  const uint8_t prev_location = this->history->get_prev_location();

  if (location < this->pages_length) {
    this->render_page(location, prev_location);
  } else {
    this->render_page(0);
  }
}

template <class Window_Page>
void Window<Window_Page>::dispatch_event(const Event *event) {
  if (!this->history) {
    this->pages[0]->dispatch_event(event);
  }

  const uint8_t location = this->history->get_location();

  if (location < this->pages_length) {
    this->pages[location]->dispatch_event(event);
  } else {
    this->pages[0]->dispatch_event(event);
  }
}

template <class Page_Component>
Page<Page_Component>::Page(Page_Component **components,
                           uint8_t components_length, uint8_t viewport_height) {
  this->components = components;
  this->components_length = components_length;
  this->viewport_height = viewport_height;
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

template <class Page_Component> void Page<Page_Component>::handle_did_mount() {
  return;
}

template <class Page_Component>
void Page<Page_Component>::handle_will_unmount() {
  return;
}

template <class Page_Component>
bool Page<Page_Component>::handle_capture_keydown(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_keydown(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_capture_keyup(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_keyup(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_capture_longkeydown(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_longkeydown(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_capture_scroll(const Event *event) {
  return true;
}

template <class Page_Component>
bool Page<Page_Component>::handle_scroll(const Event *event) {
  return true;
}

template <class Page_Component>
void Page<Page_Component>::_handle_keydown(const Event *event) {
  const bool capture_allowed = this->handle_capture_keydown(event);

  if (!capture_allowed)
    return;

  const bool propagation_allowed =
      this->components[this->focused_component]->dispatch_event(event);

  if (!propagation_allowed)
    return;

  this->handle_keydown(event);
}

template <class Page_Component>
void Page<Page_Component>::_handle_longkeydown(const Event *event) {
  const bool capture_allowed = this->handle_capture_longkeydown(event);

  if (!capture_allowed)
    return;

  const bool propagation_allowed =
      this->components[this->focused_component]->dispatch_event(event);

  if (!propagation_allowed)
    return;

  this->handle_longkeydown(event);
}

template <class Page_Component>
void Page<Page_Component>::_handle_keyup(const Event *event) {
  const bool capture_allowed = this->handle_capture_keyup(event);

  if (!capture_allowed)
    return;

  const bool propagation_allowed =
      this->components[this->focused_component]->dispatch_event(event);

  if (!propagation_allowed)
    return;

  this->handle_keyup(event);
}

template <class Page_Component>
void Page<Page_Component>::scroll(const Event *event) {
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
  }
}

template <class Page_Component>
void Page<Page_Component>::_handle_scroll(const Event *event) {
  const bool capture_allowed = this->handle_capture_scroll(event);

  if (!capture_allowed)
    return;

  const bool propagation_allowed =
      this->components[this->focused_component]->dispatch_event(event);

  if (!propagation_allowed)
    return;

  const bool handle_allowed = this->handle_scroll(event);

  if (!handle_allowed)
    return;

  this->scroll(event);
}

template <class Page_Component>
void Page<Page_Component>::dispatch_event(const Event *event) {
  if (event->type == win::KEYDOWN) {
    this->_handle_keydown(event);

    return;
  }

  if (event->type == win::KEYUP) {
    this->_handle_keyup(event);

    return;
  }

  if (event->type == win::LONGKEYDOWN) {
    this->_handle_longkeydown(event);

    return;
  }

  if (event->type == win::SCROLL) {
    this->_handle_scroll(event);

    return;
  }

  this->components[this->focused_component]->dispatch_event(event);
}

History::History(uint8_t location) { this->location = location; }

void History::push(uint8_t location) {
  this->prev_location = this->location;
  this->location = location;
}

uint8_t History::get_location() { return this->location; }

int8_t History::get_prev_location() { return this->prev_location; }

