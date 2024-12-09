#pragma once

#include "context.hpp"
#include <pugixml.hpp>

namespace fw {
struct node_data_t {
  std::optional<sf::Vector2f> position{};
  std::optional<sf::Vector2f> size{};
  std::optional<unsigned> outline_thickness{};
  std::optional<sf::Color> outline_color{};
  std::optional<sf::Color> fill_color{};
  std::optional<unsigned> text_outline_thickness{};
  std::optional<sf::Color> text_outline_color{};
  std::optional<sf::Color> text_fill_color{};
  std::optional<unsigned> text_character_size{};
  std::optional<std::string> text_string{};
  std::optional<std::string> text_font_id{};
  std::optional<float> radius{};
  std::optional<std::string> scene_to_activate{};
  std::optional<std::string> scene_to_load_src{};
  std::optional<std::string> scene_to_load_id{};
  std::optional<std::string> fill_texture_src{};
  std::optional<std::string> fill_texture_id{};
  std::optional<float> scale_x{};
  std::optional<float> scale_y{};
  std::optional<std::string> id{};
  bool exit{false};
};

struct context_t;

std::unique_ptr<node_t> build_scene(context_t *, const std::string &);

node_data_t collect_node_data(const pugi::xml_node &n, context_t *ctx);
}
