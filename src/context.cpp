#include "context.hpp"
#include "builder.hpp"
#include <filesystem>
#include <iostream>
#include <ranges>

namespace {
template <typename T> double diff(const T &n, const T &s) {
  using type = std::chrono::milliseconds;
  return std::chrono::duration_cast<type>(n - s).count();
}
} // namespace

namespace fw {
void cd_to_binary_dir(context_t *ctx, const std::string &p) {
  if (!std::filesystem::exists(p))
    return;
  std::filesystem::current_path(
      std::filesystem::absolute(std::filesystem::path{p}.parent_path()));
  ctx->current_working_dir = std::filesystem::current_path().string();
}

void update_events(context_t *ctx) {
  auto s = ctx->active_scene;

  while (auto event = ctx->window.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      for (const auto &f : s->closed_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::Resized>()) {
      for (const auto &f : s->resized_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::FocusLost>()) {
      for (const auto &f : s->focus_lost_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::FocusGained>()) {
      for (const auto &f : s->focus_gained_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::TextEntered>()) {
      for (const auto &f : s->text_entered_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::KeyPressed>()) {
      for (const auto &f : s->key_pressed_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::KeyReleased>()) {
      for (const auto &f : s->key_released_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseWheelScrolled>()) {
      for (const auto &f : s->mouse_wheel_scrolled_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseButtonPressed>()) {
      for (const auto &f : s->mouse_button_pressed_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseButtonReleased>()) {
      for (const auto &f : s->mouse_button_released_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseMoved>()) {
      for (const auto &f : s->mouse_moved_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseMovedRaw>()) {
      for (const auto &f : s->mouse_moved_raw_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseEntered>()) {
      for (const auto &f : s->mouse_entered_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::MouseLeft>()) {
      for (const auto &f : s->mouse_left_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::JoystickButtonPressed>()) {
      for (const auto &f : s->joystick_button_pressed_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::JoystickButtonReleased>()) {
      for (const auto &f : s->joystick_button_released_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::JoystickMoved>()) {
      for (const auto &f : s->joystick_moved_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::JoystickConnected>()) {
      for (const auto &f : s->joystick_connected_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::JoystickDisconnected>()) {
      for (const auto &f : s->joystick_disconnected_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::TouchBegan>()) {
      for (const auto &f : s->touch_began_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::TouchMoved>()) {
      for (const auto &f : s->touch_moved_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::TouchEnded>()) {
      for (const auto &f : s->touch_ended_event_cbs)
        if (f)
          f(s);
    } else if (event->is<sf::Event::SensorChanged>()) {
      for (const auto &f : s->sensor_changed_event_cbs)
        if (f)
          f(s);
    }
  }
}

void update(context_t *ctx) {
  if (!ctx)
    return;

  scene_t *s = ctx->active_scene;
  if (!s)
    return;

  update_events(ctx);

  auto now = std::chrono::steady_clock::now();
  if (diff(now, s->last_frame_start) < s->frame_time)
    return;

  auto ipos = sf::Mouse::getPosition(ctx->window);
  s->last_mouse_pos = sf::Vector2f{(float)ipos.x, (float)ipos.y};

  update_data_t d{};
  d.window = &ctx->window;
  d.last_mouse_pos = &s->last_mouse_pos;

  while (s->commands.size()) {
    s->commands.front()(s);
    s->commands.pop_front();
  }

  for (const auto &r : s->routines)
    r.functor(s);

  if (s->root)
    s->root->update(&d);

  s->last_frame_start = now;
}

void render(context_t *ctx) {
  if (!ctx)
    return;
  ctx->window.clear();
  if (ctx->active_scene)
    ctx->window.draw(*ctx->active_scene->root);
  ctx->window.display();
}

void activate_scene(context_t *ctx, const std::string &s) {
  if (!ctx)
    throw std::runtime_error{
        "Scene activation failed: nullptr passed as context!"};
  if (!ctx->scene_map.contains(s))
    throw std::runtime_error{
        "Scene activation failed: The requested scene does not exist!"};
  ctx->active_scene = &ctx->scene_map.at(s);
}

void remove_scene(context_t *ctx, const std::string &id,
                  bool silent_fail = false) {
  if (ctx->scene_map.contains(id)) {
    ctx->scene_map.erase(id);
    return;
  }
  if (!silent_fail)
    throw std::runtime_error{
        "A scene with the specified ID: '" + id +
        "' does not exist, yet its removal was requested!"};
}

scene_t create_scene_skeleton(context_t *ctx) {
  scene_t s{};
  s.closed_event_cbs.push_back([](auto *p) { p->window->close(); });
  s.scene_map = &ctx->scene_map;
  s.global_registers = &ctx->global_registers;
  s.current_working_dir = &ctx->current_working_dir;
  s.window = &ctx->window;
  return s;
}

void load_scene(context_t *ctx, const std::string &xml, std::string id,
                bool overwrite) {
  if (ctx->scene_map.contains(id)) {
    if (!overwrite)
      throw std::runtime_error{
          "Scene loading from the file: '" + xml +
          "' succeeded,"
          " but the overwrite parameter was set to false, yet the ID: '" +
          id + "' is not unique!"};
    remove_scene(ctx, id);
  }
  build_scene(ctx, xml, id);
}

void create_window_from_registers(context_t *ctx) {
  auto nreg = &ctx->global_registers.number;
  auto sreg = &ctx->global_registers.string;

  try {
    std::string t{sreg->get_value("window_title")};
    sf::VideoMode m{{nreg->get_value<unsigned>("window_width"),
                     nreg->get_value<unsigned>("window_height")}};

    ctx->window.create(m, t);
  } catch (...) {
    std::cerr << "Failed to fetch window parameters!" << std::endl;
    throw;
  }
}
} // namespace fw
