#include "button.hpp"
#include "resource.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

int main(int, char **argv) {
  std::filesystem::current_path(
      std::filesystem::absolute(std::filesystem::path{argv[0]}.parent_path()));

  using btype = fw::button_t<sf::RectangleShape>;
  fw::resource_t<sf::Font> font;

  if (!font.load("./font/AnonymousPro-Regular.ttf", "AnonymousPro")) {
    std::cerr << "Failed to load font!" << std::endl;
    return -1;
  }

  sf::RenderWindow win{sf::VideoMode{640, 480}, "Button demo"};
  fw::node_t scene{};

  auto btn = new btype{};
  scene.attach(std::unique_ptr<fw::node_t>{btn});
  auto s = btn->shape();
  auto l = btn->label();
  sf::Color orig{sf::Color::White};
  s->setSize(sf::Vector2f{200, 100});
  s->setFillColor(orig);
  l->setCharacterSize(30);
  l->setFont(font.get("AnonymousPro"));
  l->setFillColor(sf::Color::Black);

  btn->add_hover_cb([=](btype *ptr) {
    ptr->shape()->setFillColor(sf::Color::Yellow);
    ptr->label()->setString("Hover");
    ptr->center();
  });
  btn->add_unhover_cb([=](btype *ptr) {
    ptr->shape()->setFillColor(orig);
    ptr->label()->setString("Unhover");
    ptr->center();
  });

  btn->add_click_cb([=](btype *ptr) {
    ptr->shape()->setFillColor(sf::Color::Red);
    ptr->label()->setString("Click");
    ptr->center();
  });
  btn->add_unclick_cb([=](btype *ptr) {
    ptr->shape()->setFillColor(orig);
    ptr->label()->setString("Unclick");
    ptr->center();
  });

  fw::update_data_t data{};
  data.window = &win;
  sf::Event e{};

  auto tag = std::chrono::steady_clock::now();
  sf::Vector2f mouse_pos{};

  while (win.isOpen()) {
    while (win.pollEvent(e))
      if (e.type == sf::Event::Closed)
        win.close();

    auto tdiff = std::chrono::steady_clock::now() - tag;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(tdiff).count() >
        17) {
      tag = std::chrono::steady_clock::now();
      const auto ipos = sf::Mouse::getPosition(win);
      mouse_pos = sf::Vector2f{(float)ipos.x, (float)ipos.y};
      data.last_mouse_pos = &mouse_pos;

      float magnitude{4.f};
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        btn->move(magnitude, 0.f);
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        btn->move(-magnitude, 0.f);
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        btn->move(0.f, -magnitude);
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        btn->move(0.f, magnitude);
      scene.update(&data);
    }

    win.clear();
    win.draw(scene);
    win.display();
  }
}
