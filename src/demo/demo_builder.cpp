#include "builder.hpp"
#include "button.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

int main(int, char **argv) {
  std::filesystem::current_path(
      std::filesystem::absolute(std::filesystem::path{argv[0]}.parent_path()));

  sf::RenderWindow win{sf::VideoMode{640, 480}, "Button demo"};
  fw::context_t ctx{};
  ctx.window = &win;

  if (!ctx.font.load("./font/AnonymousPro-Regular.ttf", "AnonymousPro")) {
    std::cerr << "Failed to load font!" << std::endl;
    return -1;
  }

  auto scene = fw::build_scene("./xml/demo_button.xml", ctx);

  fw::update_data_t data{};
  data.window = &win;
  sf::Event e{};

  auto tag = std::chrono::steady_clock::now();

  while (win.isOpen()) {
    while (win.pollEvent(e))
      if (e.type == sf::Event::Closed)
        win.close();

    auto tdiff = std::chrono::steady_clock::now() - tag;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(tdiff).count() >
        17) { // cap framerate at about 60 fps
      tag = std::chrono::steady_clock::now();
      const auto ipos = sf::Mouse::getPosition(win);
      data.last_mouse_pos = sf::Vector2f{(float)ipos.x, (float)ipos.y};

      scene->update(&data);
    }

    win.clear();
    win.draw(*scene);
    win.display();
  }
}
