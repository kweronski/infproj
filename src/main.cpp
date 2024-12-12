#include "context.hpp"
#include <iostream>

int main(int, char **argv) {
  try {
    fw::context_t ctx{};
    fw::cd_to_binary_dir(&ctx, argv[0]);

    fw::load_scene(&ctx, "./xml/main_menu.xml", "main_menu");
    fw::activate_scene(&ctx, "main_menu");
    create_window_from_registers(&ctx);

    while (ctx.window.isOpen()) {
      fw::update(&ctx);
      fw::render(&ctx);
    }
  } catch (const std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}
