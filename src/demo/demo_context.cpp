/* This is the recommended demo to copy from,
 * if you'd like to add your own demo.
 */

#include "context.hpp"
#include <iostream>

namespace {
bool initialize(fw::context_t *ctx);
}

int main(int, char **argv) {
  fw::context_t ctx{}; // This holds all gui / game related data.
  fw::cd_to_binary_dir(&ctx,
                       argv[0]); // To reference resources more conveniently

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
  // The event map is scanned during each frame.
  // If a registered event has a callback, that callback is executed.
  // In this case, when the close button is pressed, the window is told to
  // close.

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
  // Here we are loading the xml files that contain descriptions from which
  // graphical objects will be assembled and linked into a tree.

  if (!ok)
    std::cerr << "Failed to initialize resources!" << std::endl;
  return ok;
}
} // namespace
