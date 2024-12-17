#include "cfusion.hpp"

int main(int, char **argv) {
  try {
    fw::context_t ctx{};
    cf::initialize(&ctx, argv[0]);

    while (ctx.window.isOpen()) {
      fw::update(&ctx);
      fw::render(&ctx);
    }
  } catch (const std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}
