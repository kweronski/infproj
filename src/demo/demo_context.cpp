#include "context.hpp"
#include <iostream>

int main(int, char **argv) {
  fw::context_t ctx{};
  fw::cd_to_binary_dir(&ctx, argv[0]);

  ctx.event_cb_map[sf::Event::Closed] = [](auto *p) { p->window.close(); };
  ctx.window.create(sf::VideoMode{640, 480}, "context demo");

  if (!ctx.font.load("./font/AnonymousPro-Regular.ttf", "AnonymousPro")) {
    std::cerr << "Failed to load font!" << std::endl;
    return -1;
  }
  fw::load_scene(&ctx, "./xml/demo_button.xml", "main_menu");
  fw::activate_scene(&ctx, "main_menu");

  while (ctx.window.isOpen()) {
    fw::update(&ctx);
    fw::render(&ctx);
  }
}
