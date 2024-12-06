#include "builder.hpp"
#include "context.hpp"
#include <iostream>
#include <optional>
#include <pugixml.hpp>
#include <unordered_map>

namespace {
using namespace fw;

struct button_data_t {
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
  bool exit{false};
};

button_data_t collect_button_data(const pugi::xml_node &n, context_t *ctx) {
  button_data_t d{};
  if (auto tag = n.child("size"); tag) {
    d.size = sf::Vector2f{};
    d.size.value().x = tag.attribute("w").as_float();
    d.size.value().y = tag.attribute("h").as_float();
  }

  if (auto tag = n.child("radius"); tag)
    d.radius = tag.attribute("value").as_float();

  if (auto tag = n.child("position"); tag) {
    d.position = sf::Vector2f{};
    if (std::string tx = tag.attribute("x").value(); tx == "center") {
      float w = d.size.has_value()     ? d.size.value().x / 2.f
                : d.radius.has_value() ? d.radius.value()
                                       : 0;
      d.position.value().x = ctx->window.getSize().x / 2.f - w;
    } else
      d.position.value().x = tag.attribute("x").as_float();
    if (std::string ty = tag.attribute("y").value(); ty == "center") {
      float h = d.size.has_value()     ? d.size.value().y / 2.f
                : d.radius.has_value() ? d.radius.value()
                                       : 0;
      d.position.value().y = ctx->window.getSize().y / 2.f - h;
    } else
      d.position.value().y = tag.attribute("y").as_float();
  }

  if (auto tag = n.child("outline_thickness"); tag)
    d.outline_thickness = tag.attribute("value").as_float();

  if (auto tag = n.child("outline_color"); tag) {
    auto r = static_cast<sf::Uint8>(tag.attribute("r").as_uint());
    auto g = static_cast<sf::Uint8>(tag.attribute("g").as_uint());
    auto b = static_cast<sf::Uint8>(tag.attribute("b").as_uint());
    d.outline_color = sf::Color{r, g, b};
  }

  if (auto tag = n.child("fill_color"); tag) {
    auto r = static_cast<sf::Uint8>(tag.attribute("r").as_uint());
    auto g = static_cast<sf::Uint8>(tag.attribute("g").as_uint());
    auto b = static_cast<sf::Uint8>(tag.attribute("b").as_uint());
    d.fill_color = sf::Color{r, g, b};
  }

  if (auto tag = n.child("text_outline_thickness"); tag)
    d.text_outline_thickness = tag.attribute("value").as_float();

  if (auto tag = n.child("text_outline_color"); tag) {
    auto r = static_cast<sf::Uint8>(tag.attribute("r").as_uint());
    auto g = static_cast<sf::Uint8>(tag.attribute("g").as_uint());
    auto b = static_cast<sf::Uint8>(tag.attribute("b").as_uint());
    d.text_outline_color = sf::Color{r, g, b};
  }

  if (auto tag = n.child("text_fill_color"); tag) {
    auto r = static_cast<sf::Uint8>(tag.attribute("r").as_uint());
    auto g = static_cast<sf::Uint8>(tag.attribute("g").as_uint());
    auto b = static_cast<sf::Uint8>(tag.attribute("b").as_uint());
    d.text_fill_color = sf::Color{r, g, b};
  }

  if (auto tag = n.child("text_character_size"); tag)
    d.text_character_size = tag.attribute("value").as_float();

  if (auto tag = n.child("text_string"); tag)
    d.text_string = tag.attribute("value").value();

  if (auto tag = n.child("text_font_id"); tag)
    d.text_font_id = tag.attribute("value").value();

  if (auto tag = n.child("activate_scene"); tag)
    d.scene_to_activate = tag.attribute("value").value();

  if (auto tag = n.child("exit"); tag)
    d.exit = true;
  return d;
}

template <typename T>
auto configure_common(const pugi::xml_node &n, context_t *ctx) {
  std::list<std::function<void(T *, context_t *)>> actions{};
  auto d = collect_button_data(n, ctx);
  if (d.position.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->shape()->setPosition(d.position.value());
    });
  if (d.outline_thickness.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->shape()->setOutlineThickness(d.outline_thickness.value());
    });
  if (d.outline_color.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->shape()->setOutlineColor(d.outline_color.value());
    });
  if (d.fill_color.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->shape()->setFillColor(d.fill_color.value());
    });
  if (d.text_outline_thickness.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->label()->setOutlineThickness(d.text_outline_thickness.value());
    });
  if (d.text_outline_color.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->label()->setOutlineColor(d.text_outline_color.value());
    });
  if (d.text_fill_color.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->label()->setFillColor(d.text_fill_color.value());
    });
  if (d.text_character_size.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->label()->setCharacterSize(d.text_character_size.value());
    });
  if (d.text_font_id.has_value())
    actions.push_back([&ctx, d](auto *ptr, auto *) {
      ptr->label()->setFont(ctx->font.get(d.text_font_id.value()));
    });
  if (d.scene_to_activate.has_value())
    actions.push_back([d](auto *, auto *ctx) {
      activate_scene(ctx, d.scene_to_activate.value());
    });
  if (d.exit)
    actions.push_back([d](auto *, auto *ctx) { ctx->window.close(); });
  return std::pair(actions, d);
}

template <typename T>
std::list<std::function<void(T *, context_t *)>>
configure_button(const pugi::xml_node &n, context_t *ctx);

using rect_b = button_t<sf::RectangleShape>;
template <>
std::list<std::function<void(rect_b *, context_t *)>>
configure_button(const pugi::xml_node &n, context_t *ctx) {
  auto [actions, d] = configure_common<rect_b>(n, ctx);
  if (d.size.has_value())
    actions.push_back(
        [d](auto *ptr, auto *) { ptr->shape()->setSize(d.size.value()); });
  if (d.text_string.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->label()->setString(d.text_string.value());
      ptr->center();
    });
  return actions;
};

using circ_b = button_t<sf::CircleShape>;
template <>
std::list<std::function<void(circ_b *, context_t *)>>
configure_button(const pugi::xml_node &n, context_t *ctx) {
  auto [actions, d] = configure_common<circ_b>(n, ctx);
  if (d.radius.has_value())
    actions.push_back(
        [d](auto *ptr, auto *) { ptr->shape()->setRadius(d.radius.value()); });
  if (d.text_string.has_value())
    actions.push_back([d](auto *ptr, auto *) {
      ptr->label()->setString(d.text_string.value());
      ptr->center();
    });
  return actions;
};

template <typename T>
std::unique_ptr<node_t> build_button_type(const pugi::xml_node &n,
                                          context_t *ctx) {
  auto ptr = new T{};
  std::unique_ptr<node_t> sptr{ptr};
  auto actions = configure_button<T>(n, ctx);
  for (const auto &e : actions)
    e(ptr, ctx);
  if (auto tag = n.child("on_hover"); tag) {
    auto a = configure_button<T>(tag, ctx);
    ptr->add_hover_cb([a, ctx](auto *p) {
      for (const auto &e : a)
        e(p, ctx);
    });
  }
  if (auto tag = n.child("on_unhover"); tag) {
    auto a = configure_button<T>(tag, ctx);
    ptr->add_unhover_cb([a, ctx](auto *p) {
      for (const auto &e : a)
        e(p, ctx);
    });
  }
  if (auto tag = n.child("on_click"); tag) {
    auto a = configure_button<T>(tag, ctx);
    ptr->add_click_cb([a, ctx](auto *p) {
      for (const auto &e : a)
        e(p, ctx);
    });
  }
  if (auto tag = n.child("on_unclick"); tag) {
    auto a = configure_button<T>(tag, ctx);
    ptr->add_unclick_cb([a, ctx](auto *p) {
      for (const auto &e : a)
        e(p, ctx);
    });
  }
  return sptr;
}
} // namespace

namespace fw {
std::unique_ptr<node_t> build_button(const pugi::xml_node &n, context_t *ctx) {
  if (auto type = std::string{n.attribute("type").value()}; type == "rectangle")
    return build_button_type<button_t<sf::RectangleShape>>(n, ctx);
  else if (type == "circle")
    return build_button_type<button_t<sf::CircleShape>>(n, ctx);
  return {};
}
} // namespace fw