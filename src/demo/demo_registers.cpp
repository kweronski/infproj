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

  if (!ctx->font.load("./font/AnonymousPro-Regular.ttf", "AnonymousPro")) {
    std::cerr << "Failed to load font!" << std::endl;
    return false;
  }

  bool ok = fw::load_scene(ctx, "./xml/demo_registers.xml", "main_menu");
  // We are referencing variables defined in the xml file, in the xml file :)
  // For example, in the beginning of the file we define the window width
  // and then we use that width in an expression.
  // To refer to a variable use the prefix __n: and __s: for numbers
  // and strings respectively.
  //
  // For example, to create a variable in the xml file:
  //
  // <nreg id="myvar" value="5"/>
  // <position x="__n:myvar * 2" y="__n:myvar + 4"/>
  //
  // would set the x component of the position to 10, and the y component to 9.
  if (!ok)
    std::cerr << "Failed to initialize resources!" << std::endl;
  else
    create_window_from_registers(ctx); // We are using values
                                       // from the xml file to configure
                                       // the window properties.
  activate_scene(ctx, "main_menu");
  return ok;
}
} // namespace
