#include "context.hpp"
#include <filesystem>
#include <iostream>

namespace {
bool load_fonts(fw::context_t *ctx);
}

int main(int, char **argv) {
  fw::context_t ctx{};
  fw::cd_to_binary_dir(&ctx, argv[0]);

  if (!load_fonts(&ctx))
    return -1;
}

namespace {
bool load_fonts(fw::context_t *ctx) {
  auto try_load = [ctx](const std::string &f, const std::string &n) {
    if (!ctx->font.load(f, n)) {
      std::cerr << "Failed to load a font from: " << f << std::endl;
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
