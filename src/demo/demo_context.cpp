#include "context.hpp"
#include <iostream>

namespace {
bool initialize(fw::context_t *ctx);
}

int main(int, char **argv) {
  fw::context_t ctx{};
  fw::cd_to_binary_dir(&ctx, argv[0]);

  if (!initialize(&ctx))
    return -1;

  while (ctx.window.isOpen()) {
    fw::update(&ctx);
    fw::render(&ctx);
  }
}

namespace {
bool initialize(fw::context_t *ctx) {
  ctx->event_cb_map[sf::Event::Closed] = [](auto *p) { p->window.close(); };
  ctx->window.create(sf::VideoMode{640, 480}, "context demo");

  if (!ctx->font.load("./font/AnonymousPro-Regular.ttf", "AnonymousPro")) {
    std::cerr << "Failed to load font!" << std::endl;
    return false;
  }

  bool ok =
      fw::load_scene(ctx, "./xml/demo_main_menu.xml", "main_menu") &&
      fw::load_scene(ctx, "./xml/demo_settings_menu.xml", "settings_menu") &&
      fw::load_scene(ctx, "./xml/demo_maze_level.xml", "maze_level") &&
      fw::activate_scene(ctx, "main_menu");

  if (!ok)
    std::cerr << "Failed to initialize resources!" << std::endl;
  return ok;
}
} // namespace
