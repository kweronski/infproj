#include "builder.hpp"
#include "evaluate.hpp"

namespace {
using namespace fw;
void collect_position(const pugi::xml_node &n, context_t *ctx, node_data_t &d) {
  auto tag = n.child("position");
  if (!tag)
    return;

  d.position = sf::Vector2f{};

  if (std::string tx = tag.attribute("x").value(); tx == "center") {
    float w = d.size.has_value()     ? d.size.value().x / 2.f
              : d.radius.has_value() ? d.radius.value()
                                     : 0;
    if (!w) {
      d.position = decltype(d.position){};
      return;
    }
    if (ctx->number_register.contains("window_width"))
      d.position.value().x = ctx->number_register.at("window_width") / 2.f - w;
    else
      d.position.value().x = ctx->window.getSize().x / 2.f - w;
  } else
    d.position.value().x =
        evaluate_expression<double>(ctx, tag.attribute("x").value());

  if (std::string ty = tag.attribute("y").value(); ty == "center") {
    float h = d.size.has_value()     ? d.size.value().y / 2.f
              : d.radius.has_value() ? d.radius.value()
                                     : 0;
    if (!h) {
      d.position = decltype(d.position){};
      return;
    }
    if (ctx->number_register.contains("window_height"))
      d.position.value().x = ctx->number_register.at("window_height") / 2.f - h;
    else
      d.position.value().y = ctx->window.getSize().y / 2.f - h;
  } else
    d.position.value().y =
        evaluate_expression<double>(ctx, tag.attribute("y").value());
}

std::optional<sf::Color> collect_color(const pugi::xml_node &n, context_t *ctx,
                                       const std::string &id) {
  if (auto tag = n.child(id.c_str()); tag) {
    auto r = evaluate_expression<sf::Uint8>(ctx, tag.attribute("r").value());
    auto g = evaluate_expression<sf::Uint8>(ctx, tag.attribute("g").value());
    auto b = evaluate_expression<sf::Uint8>(ctx, tag.attribute("b").value());
    return sf::Color{r, g, b};
  }
  return {};
}
} // namespace

namespace fw {
node_data_t collect_node_data(const pugi::xml_node &n, context_t *ctx) {
  node_data_t d{};
  if (auto tag = n.child("size"); tag) {
    d.size = sf::Vector2f{};
    d.size.value().x =
        evaluate_expression<double>(ctx, tag.attribute("w").value());
    d.size.value().y =
        evaluate_expression<double>(ctx, tag.attribute("h").value());
  }

  if (auto tag = n.child("radius"); tag)
    d.radius = evaluate_expression<double>(ctx, tag.attribute("value").value());

  collect_position(n, ctx, d);

  if (auto tag = n.child("outline_thickness"); tag)
    d.outline_thickness =
        evaluate_expression<double>(ctx, tag.attribute("value").value());

  if (auto c = collect_color(n, ctx, "outline_color"); c)
    d.outline_color = c;

  if (auto c = collect_color(n, ctx, "fill_color"); c)
    d.fill_color = c;

  if (auto tag = n.child("scale"); tag)
    d.scale_x = evaluate_expression<double>(ctx, tag.attribute("x").value());

  if (auto tag = n.child("scale"); tag)
    d.scale_y = evaluate_expression<double>(ctx, tag.attribute("y").value());

  if (auto att = n.attribute("id"); std::string{att.value()}.size())
    d.id = att.value();

  if (auto tag = n.child("fill_texture");
      tag && std::string{tag.attribute("src").value()}.size())
    d.fill_texture_src =
        evaluate_expression<std::string>(ctx, tag.attribute("src").value());

  if (auto tag = n.child("fill_texture");
      tag && std::string{tag.attribute("id").value()}.size())
    d.fill_texture_id =
        evaluate_expression<std::string>(ctx, tag.attribute("id").value());

  if (auto tag = n.child("text_outline_thickness"); tag)
    d.text_outline_thickness =
        evaluate_expression<double>(ctx, tag.attribute("value").value());

  if (auto c = collect_color(n, ctx, "text_outline_color"); c)
    d.text_outline_color = c;

  if (auto c = collect_color(n, ctx, "text_fill_color"); c)
    d.text_fill_color = c;

  if (auto tag = n.child("text_character_size"); tag)
    d.text_character_size =
        evaluate_expression<double>(ctx, tag.attribute("value").value());

  if (auto tag = n.child("text_string"); tag)
    d.text_string =
        evaluate_expression<std::string>(ctx, tag.attribute("value").value());

  if (auto tag = n.child("text_font_id"); tag)
    d.text_font_id =
        evaluate_expression<std::string>(ctx, tag.attribute("value").value());

  if (auto tag = n.child("activate_scene"); tag)
    d.scene_to_activate =
        evaluate_expression<std::string>(ctx, tag.attribute("value").value());

  if (auto tag = n.child("load_scene"); tag)
    d.scene_to_load_src =
        evaluate_expression<std::string>(ctx, tag.attribute("src").value());

  if (auto tag = n.child("load_scene"); tag)
    d.scene_to_load_id =
        evaluate_expression<std::string>(ctx, tag.attribute("id").value());

  if (auto tag = n.child("exit"); tag)
    d.exit = true;
  return d;
}
} // namespace fw
