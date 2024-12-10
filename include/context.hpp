#pragma once

#include "node.hpp"
#include "resource.hpp"
#include <chrono>
#include <functional>
#include <optional>
#include <ranges>

namespace fw {
struct context_t {
  resource_t<sf::Texture> texture{};
  resource_t<sf::Font> font{};

  sf::RenderWindow window{};

  std::chrono::time_point<std::chrono::steady_clock> frame_start{
      std::chrono::steady_clock::now()};
  double frame_time{1000.0 / 60.0};

  node_t *active_scene{};
  std::unordered_map<std::string, std::unique_ptr<node_t>> scene_map{};

  sf::Event event{};
  std::unordered_map<sf::Event::EventType, std::function<void(context_t *)>>
      event_cb_map{};

  sf::Vector2f last_mouse_pos{};
  std::string current_working_dir{};

  std::unordered_map<std::string, double> number_register{};
  std::unordered_map<std::string, std::string> string_register{};

  std::unordered_map<std::string, node_t *> vip_nodes{};

  std::list<std::function<void(context_t *)>> commands{};
  std::list<std::pair<unsigned, std::function<void(context_t *)>>> routines{};

  std::size_t routine_id_{};
};

void cd_to_binary_dir(context_t *, const std::string &);
void update(context_t *);
void render(context_t *);
bool activate_scene(context_t *, const std::string &);
bool load_scene(context_t *, const std::string &xml, std::string id);
unsigned add_routine(context_t *, std::function<void(context_t *)>);
void remove_routine(context_t *, unsigned);
void add_command(context_t *, std::function<void(context_t *)>);

template <typename T>
std::optional<T> fetch_from_register(const context_t *c,
                                     const std::string &id) {
  if (!c->number_register.contains(id))
    return {};
  return static_cast<T>(c->number_register.at(id));
}

template <>
std::optional<std::string> fetch_from_register(const context_t *c,
                                               const std::string &id);

bool create_window_from_registers(context_t *ctx);
} // namespace fw
