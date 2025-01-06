#include "cfusion.hpp"

namespace cf {
void post_process_settings_menu(fw::context_t *ctx);

void initialize(fw::context_t *ctx, const char *wdir) {
  fw::cd_to_binary_dir(ctx, wdir);

  fw::load_scene(ctx, "./xml/main_menu.xml", "main_menu");
  fw::activate_scene(ctx, "main_menu");
  create_window_from_registers(ctx);
  post_process_settings_menu(ctx);

  /* ADD YOUR CUSTOM SCENE BUILDING FUNCTION HERE */
  initialize_decrypt(ctx);
  initialize_rps(ctx);
  initialize_gomoku(ctx);
  initialize_ttt(ctx);
}

struct slider_t {
  float x, y, w, h;
  float min, max, init, inc;
  std::string variable;
  std::string font;
  std::string label;
};

template <typename T>
void create_button(fw::scene_t *s, slider_t sd, float x, std::string label,
                   float inc, fw::button_t<sf::RectangleShape> *mon) {

  auto l = new fw::button_t<sf::RectangleShape>{};
  s->root->attach(std::unique_ptr<fw::node_t>{l});

  l->shape()->setSize({50, sd.h});
  l->shape()->setPosition({sd.x + x, sd.y});
  l->shape()->setFillColor(sf::Color::White);
  l->label()->setFont(s->font.get(sd.font));
  l->label()->setCharacterSize(40);
  l->label()->setFillColor(sf::Color::Black);
  l->label()->setString(label);
  l->center();

  l->add_click_cb([s, sd, inc, mon](auto *) {
    auto var = s->global_registers->number.get_value(sd.variable);
    if (inc < 0 && var > sd.min) {
      var -= sd.inc;
      s->global_registers->number.add(sd.variable, var);
    } else if (inc > 0 && var < sd.max) {
      var += sd.inc;
      s->global_registers->number.add(sd.variable, var);
    }
    mon->label()->setString(sd.label + std::to_string(static_cast<T>(var)));
  });
};

template <typename T> void add_slider(fw::context_t *ctx, const slider_t *d) {
  auto scene = &ctx->scene_map.at("settings_menu");
  ctx->global_registers.number.add(d->variable, d->init);

  auto ptr = new fw::button_t<sf::RectangleShape>{};
  scene->root->attach(std::unique_ptr<fw::node_t>{ptr});

  ptr->shape()->setSize({d->w, d->h});
  ptr->shape()->setPosition({d->x + 50, d->y});
  ptr->shape()->setFillColor(sf::Color::Black);
  ptr->label()->setFont(scene->font.get(d->font));
  ptr->label()->setCharacterSize(40);
  ptr->label()->setFillColor(sf::Color::White);
  ptr->label()->setString(d->label + std::to_string((T)d->init));
  ptr->center();

  create_button<T>(scene, *d, 0, "-", -d->inc, ptr);
  create_button<T>(scene, *d, 50 + d->w, "+", d->inc, ptr);
}

void post_process_settings_menu(fw::context_t *ctx) {
  slider_t slider_data{.x = 400,
                       .y = 10,
                       .w = 800,
                       .h = 100,
                       .min = 1,
                       .max = 10,
                       .init = 4,
                       .inc = 1,
                       .variable = "dec_hp",
                       .font = "AnonymousPro",
                       .label = "Decrypt health points: "};
  add_slider<int>(ctx, &slider_data);

  slider_data.label = "Decrypt countdown time: ";
  slider_data.y += (slider_data.h + 10);
  slider_data.variable = "dec_cd";
  slider_data.min = 30;
  slider_data.max = 300;
  slider_data.init = 120;
  slider_data.inc = 10;
  add_slider<int>(ctx, &slider_data);

  slider_data.label = "RPS move storage: ";
  slider_data.y += (slider_data.h + 10);
  slider_data.variable = "rps_move_storage";
  slider_data.min = 5;
  slider_data.max = 100;
  slider_data.init = 20;
  slider_data.inc = 5;
  add_slider<int>(ctx, &slider_data);
}
} // namespace cf
