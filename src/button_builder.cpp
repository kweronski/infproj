#include "builder.hpp"
#include "button.hpp"
#include "context.hpp"
#include <iostream>
#include <unordered_map>

namespace fw {
node_data_t collect_node_data(const pugi::xml_node &n, scene_t *);
}

namespace {
using namespace fw;

template <typename T> inline void center(scene_t *s, button_t<T> *b) {
  sf::Vector2f wsz{};
  if (s->window->isOpen()) {
    wsz = {(float)s->window->getSize().x, (float)s->window->getSize().y};
  }
  if (!wsz.x || !wsz.y)
    wsz = sf::Vector2f{get_value_from_register<float>(s, "window_width"),
                       get_value_from_register<float>(s, "window_height")};
  auto ssz = b->bounds().value();
  b->shape()->setPosition(
      {wsz.x / 2 - ssz.width / 2, wsz.y / 2 - ssz.height / 2});
}

template <typename T>
auto configure_common(const pugi::xml_node &n, scene_t *s) {
  std::list<std::function<void(T *, context_t *, scene_t *)>> actions{};
  auto d = collect_node_data(n, s);
  if (d.position.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setPosition(d.position.value());
    });
  if (d.text_outline_color.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->label()->setOutlineColor(d.text_outline_color.value());
    });
  if (d.text_fill_color.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->label()->setFillColor(d.text_fill_color.value());
    });
  if (d.text_character_size.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->label()->setCharacterSize(d.text_character_size.value());
    });
  if (d.text_font_id.has_value())
    actions.push_back([s, d](auto *ptr, auto *, auto *) {
      ptr->label()->setFont(s->font.get(d.text_font_id.value()));
    });
  if (d.text_string.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->label()->setString(d.text_string.value());
      ptr->center();
    });
  if (d.scene_to_activate.has_value())
    actions.push_back([d](auto *, auto *ctx, auto *) {
      activate_scene(ctx, d.scene_to_activate.value());
    });
  if (d.exit)
    actions.push_back([d](auto *, auto *ctx, auto *) { ctx->window.close(); });

  if (d.bindings.size()) {
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->add_press_cb([d](auto *ptr) {
        for (const auto &b : d.bindings) {
          if (sf::Keyboard::isKeyPressed(b.key)) {
            if (b.axis == press_binding_t::axis_t::x)
              ptr->move(b.move, 0);
            else
              ptr->move(0, b.move);
          }
        }
      });
    });
  }
  return std::pair(actions, d);
}

template <typename T>
std::list<std::function<void(T *, context_t *, scene_t *)>>
configure_button(const pugi::xml_node &n, scene_t *);

using rect_b = button_t<sf::RectangleShape>;
template <>
std::list<std::function<void(rect_b *, context_t *, scene_t *)>>
configure_button(const pugi::xml_node &n, scene_t *s) {
  auto [actions, d] = configure_common<rect_b>(n, s);
  if (d.size.has_value())
    actions.push_front([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setSize(d.size.value());
    });
  if (d.fill_color.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setFillColor(d.fill_color.value());
    });
  if (d.outline_color.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setOutlineColor(d.outline_color.value());
    });
  if (d.text_outline_thickness.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->label()->setOutlineThickness(d.text_outline_thickness.value());
    });
  if (d.outline_thickness.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setOutlineThickness(d.outline_thickness.value());
    });
  return actions;
};

using circ_b = button_t<sf::CircleShape>;
template <>
std::list<std::function<void(circ_b *, context_t *, scene_t *)>>
configure_button(const pugi::xml_node &n, scene_t *s) {
  auto [actions, d] = configure_common<circ_b>(n, s);
  if (d.radius.has_value())
    actions.push_front([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setRadius(d.radius.value());
    });
  if (d.fill_color.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setFillColor(d.fill_color.value());
    });
  if (d.outline_color.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setOutlineColor(d.outline_color.value());
    });
  if (d.text_outline_thickness.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->label()->setOutlineThickness(d.text_outline_thickness.value());
    });
  if (d.outline_thickness.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setOutlineThickness(d.outline_thickness.value());
    });
  return actions;
};

using sprite_b = button_t<sf::Sprite>;
template <>
std::list<std::function<void(sprite_b *, context_t *, scene_t *)>>
configure_button(const pugi::xml_node &n, scene_t *scene) {
  auto [actions, d] = configure_common<sprite_b>(n, scene);
  if (d.fill_texture_id.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *s) {
      ptr->shape()->setTexture(s->texture.get(d.fill_texture_id.value()));
    });
  if (d.fill_texture_src.has_value() && d.fill_texture_id.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *s) {
      s->texture.load(d.fill_texture_src.value(), d.fill_texture_id.value());
      ptr->shape()->setTexture(s->texture.get(d.fill_texture_id.value()));
    });
  if (d.scale_x.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setScale(d.scale_x.value(), ptr->shape()->getScale().y);
    });
  if (d.scale_y.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *) {
      ptr->shape()->setScale(ptr->shape()->getScale().x, d.scale_y.value());
    });
  if (!d.position.has_value())
    actions.push_back([d](auto *ptr, auto *, auto *s) { center(s, ptr); });
  return actions;
};

template <typename T>
std::unique_ptr<node_t> build_button_type(const pugi::xml_node &n,
                                          context_t *ctx, scene_t *s) {
  auto ptr = new T{};
  std::unique_ptr<node_t> sptr{ptr};

  auto actions = configure_button<T>(n, s);
  for (const auto &e : actions)
    e(ptr, ctx, s);

  if (auto tag = n.child("on_hover"); tag && tag.first_child()) {
    auto a = configure_button<T>(tag, s);
    ptr->add_hover_cb([a, ctx, s](auto *p) {
      for (const auto &e : a)
        e(p, ctx, s);
    });
  }
  if (auto tag = n.child("on_unhover"); tag && tag.first_child()) {
    auto a = configure_button<T>(tag, s);
    ptr->add_unhover_cb([a, ctx, s](auto *p) {
      for (const auto &e : a)
        e(p, ctx, s);
    });
  }
  if (auto tag = n.child("on_click"); tag && tag.first_child()) {
    auto a = configure_button<T>(tag, s);
    ptr->add_click_cb([a, ctx, s](auto *p) {
      for (const auto &e : a)
        e(p, ctx, s);
    });
  }
  if (auto tag = n.child("on_unclick"); tag && tag.first_child()) {
    auto a = configure_button<T>(tag, s);
    ptr->add_unclick_cb([a, ctx, s](auto *p) {
      for (const auto &e : a)
        e(p, ctx, s);
    });
  }
  for (auto tag = n.child("on_touch"); tag && tag.first_child();
       tag = tag.next_sibling("on_touch")) {
    std::string target = tag.attribute("id").value();
    if (!target.size())
      throw std::runtime_error{
          "Invalid use of on_touch tag. An id is required!"};

    auto a = configure_button<T>(tag, s);
    add_routine(s, [a, ctx, ptr, target](auto *s) {
      if (auto tb = s->vip_nodes.at(target)->bounds(); tb.has_value()) {
        if (auto pb = ptr->bounds(); pb.has_value())
          if (tb.value().intersects(pb.value()))
            for (const auto &e : a)
              e(ptr, ctx, s);
      }
    });
  }
  return sptr;
}
} // namespace

namespace fw {
std::unique_ptr<node_t> build_button(const pugi::xml_node &n, context_t *ctx,
                                     scene_t *s) {
  std::unique_ptr<node_t> node{};
  if (auto type = std::string{n.attribute("type").value()}; type == "rectangle")
    node = build_button_type<button_t<sf::RectangleShape>>(n, ctx, s);
  else if (type == "circle")
    node = build_button_type<button_t<sf::CircleShape>>(n, ctx, s);
  else if (type == "sprite")
    node = build_button_type<button_t<sf::Sprite>>(n, ctx, s);
  return node;
}
} // namespace fw
