#pragma once
#include "node.hpp"
#include "register.hpp"
#include "resource.hpp"
#include <chrono>
#include <functional>

namespace fw {
struct scene_t {
  // Elements accessible from context
  std::unordered_map<std::string, scene_t> *scene_map{};
  register_pool_t *global_registers{};
  std::string *current_working_dir{};
  sf::RenderWindow *window{};

  // Elements unique to scene
  using functor_t = std::function<void(scene_t *)>;
  struct routine_t {
    functor_t functor{};
    std::size_t id{};
  };

  register_pool_t local_registers{};

  resource_t<sf::Texture> texture{};
  resource_t<sf::Font> font{};

  std::unique_ptr<node_t> root{new node_t{}};
  sf::Vector2f last_mouse_pos{};

  std::unordered_map<std::string, node_t *> vip_nodes{};

  decltype(routine_t::id) routine_id_{};
  std::list<routine_t> routines{};
  std::list<functor_t> commands{};

  std::chrono::time_point<std::chrono::steady_clock> last_frame_start{};
  double frame_time{1000.0 / 60.0};

  sf::Event event{};
  std::unordered_map<sf::Event::EventType, functor_t> event_cb_map{};
};

template <typename T>
T get_value_from_register(const scene_t *s, const std::string &id) {
  if (!s)
    throw std::runtime_error{"The scene pointer is nullptr!"};
  if (s->local_registers.number.get_pointer(id))
    return s->local_registers.number.get_value<T>(id);
  if (s->global_registers->number.get_pointer(id))
    return s->global_registers->number.get_value<T>(id);
  throw std::runtime_error{"Fetching number from register with id: '" + id +
                           "' failed."};
}

template <>
inline std::string get_value_from_register(const scene_t *s,
                                           const std::string &id) {
  if (!s)
    throw std::runtime_error{"The scene pointer is nullptr!"};
  if (s->local_registers.string.get_pointer(id))
    return s->local_registers.string.get_value<std::string>(id);
  if (s->global_registers->string.get_pointer(id))
    return s->global_registers->string.get_value<std::string>(id);
  throw std::runtime_error{"Fetching string from register with id: '" + id +
                           "' failed."};
}

unsigned add_routine(scene_t *, std::function<void(scene_t *)>);
void remove_routine(scene_t *, unsigned);

void add_command(scene_t *, std::function<void(scene_t *)>);
} // namespace fw
