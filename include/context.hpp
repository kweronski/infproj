#include "resource.hpp"
#include <chrono>
#include <functional>

namespace fw {
struct context_t {
  resource_t<sf::Font> texture;
  resource_t<sf::Font> font;

  sf::RenderWindow window;

  std::chrono::time_point<std::chrono::steady_clock> frame_start;
  double frame_time;

  node_t *active_scene;
  std::unordered_map<std::string, std::unique_ptr<node_t>> scene_map;
};

} // namespace fw
