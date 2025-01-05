#include "gomoku.hpp"
#include "button.hpp"
#include "context.hpp"

void initialize_gomoku(fw::context_t *ctx) {
  auto s = fw::create_scene_skeleton(ctx);
  auto wh = fw::get_value_from_register<float>(&s, "window_height");
  auto ww = fw::get_value_from_register<float>(&s, "window_width");
  const std::string sn{"gomoku"};
  if (ctx->scene_map.contains(sn))
    ctx->scene_map.erase(sn);

  s.texture.load("table_center", "./img/gomoku_table_center.png");
  s.texture.load("white_stone", "./img/gomoku_white_stone.png");
  s.texture.load("black_stone", "./img/gomoku_black_stone.png");
  s.font.load("CourierPrime", "./font/CourierPrime.ttf");
  auto create_label = [&s](std::string t, std::string i,
                           sf::Color c = sf::Color::White, unsigned sz = 60) {
    auto ptr = new fw::basic_node_t<sf::Text>{};
    s.root->attach(std::unique_ptr<fw::node_t>{ptr});
    ptr->shape().reset(new sf::Text{s.font.get_first()});

    ptr->shape()->setFont(s.font.get("CourierPrime"));
    ptr->shape()->setCharacterSize(sz);
    ptr->shape()->setFillColor(c);
    ptr->shape()->setString(t);
    if (i.size())
      s.vip_nodes.emplace(i, ptr);
    return ptr;
  };

  auto rootptr = new gomoku_root_t{};
  s.root = std::unique_ptr<::fw::node_t>{rootptr};

  const float offset_x = ww * 0.289; // BOARD_X
  const float offset_y = wh * 0.125; // BOARD_Y

  auto find_button_nr = [wh, offset_x, offset_y](const sf::Vector2i pos) {
    int x = (pos.x - offset_x) / (wh / 20.f);
    int y = (pos.y - offset_y) / (wh / 20.f);
    if ((pos.x > offset_x + (wh / 20.f) * 15) || (pos.x < offset_x) ||
        (pos.y < offset_y) || (pos.y > offset_y + (wh / 20.f) * 15))
      return -1;
    return y * 15 + x;
  };

  auto evaluate_win = [rootptr](int button) {
    const auto &ownership = rootptr->ownership;
    const auto player = rootptr->player;
    const int directions_a[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                    {0, 1},   {1, -1}, {1, 0},  {1, 1}};
    const int directions_b[8][2] = {{1, 1},  {1, 0},  {1, -1}, {0, 1},
                                    {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}};
    for (int i = 0; i < 8; i++) {
      int x_a = button % 15;
      int y_a = button / 15;
      int a_ok = 0;

      int x_b = button % 15;
      int y_b = button / 15;
      int b_ok = 0;

      while (ownership[y_a * 15 + x_a] == player) {
        y_a += directions_a[i][0];
        x_a += directions_a[i][1];
        a_ok++;
      }

      while (ownership[y_b * 15 + x_b] == player) {
        y_b += directions_b[i][0];
        x_b += directions_b[i][1];
        b_ok++;
      }

      if (a_ok + b_ok > 5)
        return player;
    }
    return 0;
  };

  auto reset = [rootptr, ctx]() {
    auto s = ctx->active_scene;
    rootptr->winner = 0;
    rootptr->move = 0;
    rootptr->draw = 0;

    for (int i = 0; i < 15 * 15; i++) {
      rootptr->ownership[i] = 0;
    }

    for (int i = 0; i < 15 * 15 / 2; i++) {
      s->vip_nodes.at("w" + std::to_string(i))->hide();
    }

    for (int i = 0; i < 15 * 15 / 2; i++) {
      s->vip_nodes.at("b" + std::to_string(i))->hide();
    }

    s->vip_nodes.at("player_won")->hide();
    s->vip_nodes.at("end_screen")->hide();
    s->vip_nodes.at("draw")->hide();
    s->vip_nodes.at("reset_l")->hide();
  };

  // PROPOZYCJA DODANIA GUZIKA na powrot
  auto backb = new fw::button_t<sf::RectangleShape>{};
  s.root->attach(std::unique_ptr<fw::node_t>{backb});
  backb->label().reset(new sf::Text{s.font.get_first()});
  backb->shape().reset(new sf::RectangleShape{});

  backb->label()->setFont(s.font.get("CourierPrime"));
  backb->label()->setCharacterSize(60);
  backb->label()->setFillColor(sf::Color::White);
  backb->label()->setString("Back");
  backb->shape()->setSize({200.f, 100.f});
  backb->shape()->setPosition({(ww - 200.f) / 2.f, wh - 110.f});
  backb->shape()->setFillColor(sf::Color::Black);
  backb->shape()->setOutlineColor(sf::Color::White);
  backb->shape()->setOutlineThickness(5);
  backb->center();

  backb->add_hover_cb(
      [](auto *ptr) { ptr->shape()->setOutlineColor(sf::Color::Red); });
  backb->add_unhover_cb(
      [](auto *ptr) { ptr->shape()->setOutlineColor(sf::Color::White); });

  backb->add_click_cb([ctx, reset](auto *) {
    reset();
    fw::activate_scene(ctx, "selection_menu");
  });

  auto make_new_pos = [offset_x, offset_y, wh](int i) {
    return sf::Vector2f{offset_x + int(i % 15) * int(wh / 20),
                        offset_y + int(i / 15) * int(wh / 20)};
  };

  auto interact = [evaluate_win, rootptr, ctx, make_new_pos,
                   reset](int i = -1) {
    auto s = ctx->active_scene;
    if (i == -1)
      return;
    if (rootptr->winner > 0 || rootptr->draw) {
      reset();
      return;
    }
    if (rootptr->move > (15 * 15 - 1)) {
      s->vip_nodes.at("end_screen")->show();
      s->vip_nodes.at("draw")->show();
      s->vip_nodes.at("reset_l")->show();
      rootptr->draw=1;
      return;
    }
    if (rootptr->winner == 0)
      if (rootptr->ownership[i] == 0) {
        rootptr->ownership[i] = rootptr->player;
        rootptr->winner = evaluate_win(i);
        rootptr->move++;
        switch (rootptr->player) {
        case 1:
          dynamic_cast<fw::button_t<sf::Sprite> *>(
              s->vip_nodes.at("w" + std::to_string(rootptr->move / 2)))
              ->show();
          dynamic_cast<fw::button_t<sf::Sprite> *>(
              s->vip_nodes.at("w" + std::to_string(rootptr->move / 2)))
              ->shape()
              ->setPosition(make_new_pos(i));
          break;
        case 2:
          dynamic_cast<fw::button_t<sf::Sprite> *>(
              s->vip_nodes.at("b" + std::to_string(rootptr->move / 2)))
              ->show();
          dynamic_cast<fw::button_t<sf::Sprite> *>(
              s->vip_nodes.at("b" + std::to_string(rootptr->move / 2)))
              ->shape()
              ->setPosition(make_new_pos(i));
          break;
        }
        rootptr->player ^= 3;
      }
    if (rootptr->winner > 0) {
      rootptr->wins[rootptr->winner - 1]++;
      s->vip_nodes.at("end_screen")->show();
      s->vip_nodes.at("player_won")->show();
      s->vip_nodes.at("reset_l")->show();
      dynamic_cast<fw::basic_node_t<sf::Text> *>(s->vip_nodes.at("player_won"))
          ->shape()
          ->setString("Player " + std::to_string(rootptr->winner) + " won!");

      dynamic_cast<fw::basic_node_t<sf::Text> *>(s->vip_nodes.at("wins1"))
          ->shape()
          ->setString(std::to_string(rootptr->wins[0]));

      dynamic_cast<fw::basic_node_t<sf::Text> *>(s->vip_nodes.at("wins2"))
          ->shape()
          ->setString(std::to_string(rootptr->wins[1]));
    }
  };

  s.mouse_button_released_event_cbs.push_back(
      [interact, find_button_nr](auto *s) {
        interact(find_button_nr(sf::Mouse::getPosition(*s->window)));
      });

  auto create_button = [&s](float x, float y, std::string t, std::string i) {
    auto ptr = new fw::button_t<sf::Sprite>{};
    s.root->attach(std::unique_ptr<fw::node_t>{ptr});
    ptr->shape().reset(new sf::Sprite{s.texture.get(t)});
    ptr->label().reset(new sf::Text{s.font.get_first()});
    ptr->shape()->setPosition(sf::Vector2f{x, y});
    if (i.size())
      s.vip_nodes.emplace(i, ptr);
    return ptr;
  };

  for (int i = 0; i < 15 * 15; i++) {
    create_button(ww * 0.289 + (i % 15) * wh / 20,
                  (int)wh * 0.125 + (int(i / 15) * (wh / 20)), "table_center",
                  std::to_string(i));
  }

  for (int i = 0; i < 15 * 15 / 2; i++) {
    create_button(0, 0, "white_stone", "w" + std::to_string(i));
  }

  for (int i = 0; i < 15 * 15 / 2; i++) {
    create_button(0, 0, "black_stone", "b" + std::to_string(i));
  }

  for (int i = 0; i < 15 * 15 / 2; i++) {
    dynamic_cast<fw::button_t<sf::Sprite> *>(
        s.vip_nodes.at("b" + std::to_string(i)))
        ->hide();
    dynamic_cast<fw::button_t<sf::Sprite> *>(
        s.vip_nodes.at("w" + std::to_string(i)))
        ->hide();
  }

  auto wins1 = create_label("0", "wins1");
  wins1->shape()->setPosition({50, 50});
  auto wins2 = create_label("0", "wins2");
  wins2->shape()->setPosition({1830, 50});

  auto ptr = new fw::button_t<sf::RectangleShape>{};
  s.root->attach(std::unique_ptr<fw::node_t>{ptr});
  ptr->label().reset(new sf::Text{s.font.get_first()});
  ptr->shape().reset(new sf::RectangleShape{});

  ptr->shape()->setSize({ww, wh});
  ptr->shape()->setFillColor(sf::Color{50, 50, 50, 120});
  ptr->hide();
  s.vip_nodes.emplace("end_screen", ptr);
  create_label("Player _ won!", "player_won");
  dynamic_cast<fw::basic_node_t<sf::Text> *>(s.vip_nodes.at("player_won"))
      ->shape()
      ->setPosition({700, 400});
  dynamic_cast<fw::basic_node_t<sf::Text> *>(s.vip_nodes.at("player_won"))
      ->hide();

  create_label("Game endend in a draw!", "draw");
  dynamic_cast<fw::basic_node_t<sf::Text> *>(s.vip_nodes.at("draw"))
      ->shape()
      ->setPosition({600, 400});
  dynamic_cast<fw::basic_node_t<sf::Text> *>(s.vip_nodes.at("draw"))->hide();

  create_label("Press anywhere to reset...", "reset_l");
  s.vip_nodes.at("reset_l")->hide();
  dynamic_cast<fw::basic_node_t<sf::Text> *>(s.vip_nodes.at("reset_l"))
      ->shape()
      ->setPosition({570, 500});

  ctx->scene_map.emplace(std::move(sn), std::move(s));
}
