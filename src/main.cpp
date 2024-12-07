#include "context.hpp"
#include <filesystem>
#include <iostream>

namespace {
bool initialize(fw::context_t *ctx);
bool load_fonts(fw::context_t *ctx);
bool load_scenes(fw::context_t *ctx);
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
  if (!load_fonts(ctx)) {
    std::cerr << "Failed to load fonts!" << std::endl;
    return false;
  }

  if (!load_scenes(ctx)) {
    std::cerr << "Failed to load scenes!" << std::endl;
    return false;
  }

  if (!fw::activate_scene(ctx, "main_menu")) {
    std::cerr << "Failed to activate the initial scene!" << std::endl;
    return false;
  }

  create_window_from_registers(ctx);
  return true;
}

bool load_scenes(fw::context_t *ctx) {
  auto try_load = [ctx](const std::string &f, const std::string &n) {
    if (!fw::load_scene(ctx, f, n)) {
      std::cerr << "Failed to load: " << f << std::endl;
      return false;
    }
    return true;
  };
  bool ok = try_load("./xml/main_menu.xml", "main_menu") &&
            try_load("./xml/selection_menu.xml", "selection_menu") &&
            try_load("./xml/settings_menu.xml", "settings_menu");
  return ok;
}

bool load_fonts(fw::context_t *ctx) {
  auto try_load = [ctx](const std::string &f, const std::string &n) {
    if (!ctx->font.load(f, n)) {
      std::cerr << "Failed to load: " << f << std::endl;
      return false;
    }
    return true;
  };

  bool ok = try_load("./font/AnonymousPro-Regular.ttf", "AnonymousPro") &&
            try_load("./font/CourierPrime.ttf", "CourierPrime") &&
            try_load("./font/digital-7.ttf", "Digital7") &&
            try_load("./font/Inconsolata-Regular.ttf", "Inconsolata") &&
            try_load("./font/JetBrainsMono-Regular.ttf", "JetBrainsMono") &&
            try_load("./font/MerchantCopy.ttf", "MerchantCopy") &&
            try_load("./font/ninepin.ttf", "NinePin") &&
            try_load("./font/november.ttf", "November") &&
            try_load("./font/SophieMonoSerif.ttf", "SophieMonoSerif") &&
            try_load("./font/SourceCodePro-Regular.ttf", "SourceCodePro");

  return ok;
}
} // namespace
