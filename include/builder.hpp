#pragma once

#include "context.hpp"
#include <pugixml.hpp>

namespace fw {
struct press_binding_t {
  sf::Keyboard::Key key;
  float move;
  enum class axis_t { x, y };
  axis_t axis;
};

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
  std::optional<std::string> fill_texture_src{};
  std::optional<std::string> fill_texture_id{};
  std::optional<float> scale_x{};
  std::optional<float> scale_y{};
  std::optional<std::string> id{};
  std::list<press_binding_t> bindings{};
  bool exit{false};
};

struct context_t;

void build_scene(context_t *, const std::string &xml, const std::string &id);
}
