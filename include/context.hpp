#include "node.hpp"
#include "resource.hpp"
#include <chrono>
#include <functional>

namespace fw {
struct context_t {
  resource_t<sf::Font> texture{};
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
};

void cd_to_binary_dir(context_t *, const std::string &);
void update(context_t *);
void render(context_t *);
void activate_scene(context_t *, const std::string &);
bool load_scene(context_t *, const std::string &xml, const std::string id);
} // namespace fw
