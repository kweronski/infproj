#include "node.hpp"
#include "resource.hpp"
#include <filesystem>
#include <iostream>

struct resource {
  static inline fw::resource_t<sf::Font> font;
  static inline fw::resource_t<sf::Texture> texture;
};

int main(int argc, char **argv) {
  std::filesystem::current_path(
      std::filesystem::absolute(std::filesystem::path{argv[0]}.parent_path()));

  resource::font.load("./font/AnonymousPro-Regular.ttf", "AnonymousPro");
  resource::font.load("./font/CourierPrime.ttf", "CourierPrime");
  resource::font.load("./font/digital-7.ttf", "Digital7");
  resource::font.load("./font/Inconsolata-Regular.ttf", "Inconsolata");
  resource::font.load("./font/JetBrainsMono-Regular.ttf", "JetBrainsMono");
  resource::font.load("./font/MerchantCopy.ttf", "MerchantCopy");
  resource::font.load("./font/ninepin.ttf", "NinePin");
  resource::font.load("./font/november.ttf", "November");
  resource::font.load("./font/SophieMonoSerif.ttf", "SophieMonoSerif");
  resource::font.load("./font/SourceCodePro-Regular.ttf", "SourceCodePro");
}
