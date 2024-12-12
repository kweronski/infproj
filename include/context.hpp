#pragma once

#include "scene.hpp"

namespace fw {
struct context_t {
  std::unordered_map<std::string, scene_t> scene_map{};
  scene_t *active_scene{};

  std::string current_working_dir{};
  sf::RenderWindow window{};

  // global registers
  register_pool_t global_registers{};
};

void cd_to_binary_dir(context_t *, const std::string &);
void create_window_from_registers(context_t *ctx);

void update(context_t *);
void render(context_t *);

void activate_scene(context_t *, const std::string &);
void load_scene(context_t *, const std::string &xml, std::string id,
                bool overwrite = true);

scene_t create_scene_skeleton(context_t *);
} // namespace fw
