#include "ttt.hpp"
#include <thread>
#include <unordered_map>

namespace cf {
void initialize_ttt(fw::context_t *ctx) {
  auto s = fw::create_scene_skeleton(ctx);
  auto wh = fw::get_value_from_register<float>(&s, "window_height");
  auto ww = fw::get_value_from_register<float>(&s, "window_width");

  const std::string sn{"tic_tac_toe"};
  if (ctx->scene_map.contains(sn))
    ctx->scene_map.erase(sn);

  s.texture.load("x_logo", "./img/ttt_x.png");
  s.texture.load("o_logo", "./img/ttt_o.png");
  s.texture.load("board_logo", "./img/ttt_board.png");
  s.font.load("Digital7", "./font/digital-7.ttf");
  s.font.load("AnonymousPro", "./font/AnonymousPro-Regular.ttf");

  auto tttptr = new ttt_t{};
  s.root = std::unique_ptr<::fw::node_t>{tttptr};
  init_graph(&tttptr->graph);

/*
  fw::add_routine(&s, [tttptr](auto *scene) {
    if (!fw::get_value_from_register<float>(scene, "rps_update_settings"))
      return;
    scene->global_registers->number.add("rps_update_settings", 0);
    tttptr->recent_moves.resize(
        fw::get_value_from_register<float>(scene, "rps_move_storage"));
  });
*/
  auto create_label = [&s](std::string t, std::string i,
                           sf::Color c = sf::Color::White, unsigned sz = 60) {
    auto ptr = new fw::basic_node_t<sf::Text>{};
    s.root->attach(std::unique_ptr<fw::node_t>{ptr});
    ptr->shape()->setFont(s.font.get("Digital7"));
    ptr->shape()->setCharacterSize(sz);
    ptr->shape()->setFillColor(c);
    ptr->shape()->setString(t);
    if (i.size())
      s.vip_nodes.emplace(i, ptr);
    return ptr;
  };

  auto rtext = create_label("ROUND: 1", "round_counter");
  auto bb = rtext->shape()->getGlobalBounds();
  rtext->shape()->setPosition({(ww - bb.width) / 2.f, bb.height});

  auto rscore = create_label("SCORE: 0/0", "score_counter");
  bb = rscore->shape()->getGlobalBounds();
  rscore->shape()->setPosition({(ww - bb.width) / 2.f, 3 * bb.height});

  auto rstat = create_label("STATUS:", "round_status");
  bb = rstat->shape()->getGlobalBounds();
  rstat->shape()->setPosition({(ww - bb.width) / 2.f, 5 * bb.height});

  auto win = create_label("YOU WIN!", "victory", sf::Color::Green);
  bb = win->shape()->getGlobalBounds();
  win->shape()->setPosition({ww / 2.f + bb.width * 1.8f, 5.f * bb.height - 5});
  win->hide();

  auto def = create_label("YOU LOSE!", "defeat", sf::Color::Red);
  bb = def->shape()->getGlobalBounds();
  def->shape()->setPosition({ww / 2.f + bb.width * 1.3f, 5.f * bb.height});
  def->hide();

  auto tie = create_label("IT'S A TIE!", "tie", sf::Color::Yellow);
  bb = tie->shape()->getGlobalBounds();
  tie->shape()->setPosition({ww / 2.f + bb.width * 1.3f, 5.f * bb.height + 5});
  tie->hide();

  auto vs = create_label("VS", "vs", sf::Color::White, 100);
  bb = vs->shape()->getGlobalBounds();
  vs->shape()->setPosition({1325, 550});
  vs->hide();

  auto create_icon = [&s](float x, float y, std::string t, std::string i) {
    auto ptr = new fw::button_t<sf::Sprite>{};
    s.root->attach(std::unique_ptr<fw::node_t>{ptr});
    ptr->shape()->setPosition(sf::Vector2f{x, y});
    ptr->shape()->setTexture(s.texture.get(t));
    ptr->shape()->setScale(0.5, 0.5);
    if (i.size())
      s.vip_nodes.emplace(i, ptr);
    return ptr;
  };

  auto hide_routine = [ctx]() {
    auto s = ctx->active_scene;
    s->vip_nodes.at("victory")->hide();
    s->vip_nodes.at("defeat")->hide();
    s->vip_nodes.at("tie")->hide();
    s->vip_nodes.at("vs")->hide();
    s->vip_nodes.at("prock")->hide();
    s->vip_nodes.at("ppaper")->hide();
    s->vip_nodes.at("pscissors")->hide();
    s->vip_nodes.at("oprock")->hide();
    s->vip_nodes.at("oppaper")->hide();
    s->vip_nodes.at("opscissors")->hide();
  };

  auto show_icon = [ctx](ttt_t::move_t m, std::string p) {
    auto s = ctx->active_scene;
    switch (m) {
    case ttt_t::move_t::rock:
      s->vip_nodes.at(p + "rock")->show();
      break;
    case ttt_t::move_t::paper:
      s->vip_nodes.at(p + "paper")->show();
      break;
    case ttt_t::move_t::scissors:
      s->vip_nodes.at(p + "scissors")->show();
      break;
    default:
      throw std::runtime_error{"We shouldn't be here!"};
    }
  };

  auto update_round = [ctx](auto *rps) {
    auto s = ctx->active_scene;
    s->vip_nodes.at("vs")->show();
    auto rnum = std::to_string(++rps->round);
    dynamic_cast<fw::basic_node_t<sf::Text> *>(s->vip_nodes.at("round_counter"))
        ->shape()
        ->setString("ROUND: " + rnum);
  };

  auto update_status = [ctx, tttptr, show_icon](ttt_t::move_t m) {
    auto s = ctx->active_scene;
    register_move(m, tttptr);
    auto ai = ai_move(tttptr);
    show_icon(m, "p");   // player prefix
    show_icon(ai, "op"); // opponent prefix
    switch (victor(m, ai, &tttptr->graph)) {
    case 0:
      s->vip_nodes.at("tie")->show();
      break;
    case 1: {
      s->vip_nodes.at("victory")->show();
      auto sc = std::to_string(++tttptr->score);
      auto ai = std::to_string(tttptr->aiscore);
      dynamic_cast<fw::basic_node_t<sf::Text> *>(
          s->vip_nodes.at("score_counter"))
          ->shape()
          ->setString("SCORE: " + sc + "/" + ai);
    } break;
    case 2: {
      s->vip_nodes.at("defeat")->show();
      auto sc = std::to_string(tttptr->score);
      auto ai = std::to_string(++tttptr->aiscore);
      dynamic_cast<fw::basic_node_t<sf::Text> *>(
          s->vip_nodes.at("score_counter"))
          ->shape()
          ->setString("SCORE: " + sc + "/" + ai);
    } break;
    default:
      throw std::runtime_error{"Something went wrong!"};
    }
  };

  auto update = [&s, hide_routine, show_icon, update_status, tttptr,
                 update_round](ttt_t::move_t m) {
    hide_routine();
    update_round(tttptr);
    update_status(m);
  };

  auto rockb = create_icon(100, 200, "rock_logo", "rock_button");
  rockb->add_click_cb([update](auto *) { update(ttt_t::move_t::rock); });

  auto paperb = create_icon(100, 600, "paper_logo", "paper_button");
  ;
  paperb->add_click_cb([update](auto *) { update(ttt_t::move_t::paper); });

  auto scissorb = create_icon(350, 400, "scissors_logo", "scissors_button");
  scissorb->add_click_cb([update](auto *) { update(ttt_t::move_t::scissors); });

  auto pr = create_icon(1000, 500, "rock_logo", "prock");
  pr->hide();
  auto pp = create_icon(1000, 500, "paper_logo", "ppaper");
  ;
  pp->hide();
  auto ps = create_icon(1000, 500, "scissors_logo", "pscissors");
  ps->hide();

  auto opr = create_icon(1500, 500, "rock_logo", "oprock");
  opr->hide();
  auto opp = create_icon(1500, 500, "paper_logo", "oppaper");
  ;
  opp->hide();
  auto ops = create_icon(1500, 500, "scissors_logo", "opscissors");
  ops->hide();

  auto backb = new fw::button_t<sf::RectangleShape>{};
  s.root->attach(std::unique_ptr<fw::node_t>{backb});
  backb->label()->setFont(s.font.get("AnonymousPro"));
  backb->label()->setCharacterSize(60);
  backb->label()->setFillColor(sf::Color::White);
  backb->label()->setString("Back");
  backb->shape()->setSize({200.f, 100.f});
  backb->shape()->setPosition((ww - 200.f) / 2.f, wh - 110.f);
  backb->shape()->setFillColor(sf::Color::Black);
  backb->shape()->setOutlineColor(sf::Color::White);
  backb->shape()->setOutlineThickness(5);
  backb->center();
  backb->add_hover_cb(
      [](auto *ptr) { ptr->shape()->setOutlineColor(sf::Color::Red); });
  backb->add_unhover_cb(
      [](auto *ptr) { ptr->shape()->setOutlineColor(sf::Color::White); });
  backb->add_click_cb([ctx, hide_routine, tttptr](auto *) {
    auto s = ctx->active_scene;
    hide_routine();
    tttptr->round = 1;
    tttptr->score = 0;
    tttptr->aiscore = 0;
    dynamic_cast<fw::basic_node_t<sf::Text> *>(s->vip_nodes.at("score_counter"))
        ->shape()
        ->setString("SCORE: 0/0");
    dynamic_cast<fw::basic_node_t<sf::Text> *>(s->vip_nodes.at("round_counter"))
        ->shape()
        ->setString("ROUND: 1");
    for (auto &e : tttptr->recent_moves)
      e = ttt_t::move_t::zero;
    fw::activate_scene(ctx, "selection_menu");
  });
  ctx->scene_map.emplace(std::move(sn), std::move(s));
}

void init_graph(ttt_t::element_t *rock) {
  if (!rock)
    throw std::runtime_error{"Passed nullptr to rps list head!"};
  rock->wins_against.clear();
  rock->defeated_by.clear();

  auto scissors = std::make_unique<ttt_t::element_t>();
  scissors->type = ttt_t::move_t::scissors;

  auto paper = std::make_unique<ttt_t::element_t>();
  paper->type = ttt_t::move_t::paper;

  rock->type = ttt_t::move_t::rock;

  rock->wins_against.push_back(scissors.get());
  rock->defeated_by.push_back(paper.get());

  paper->wins_against.push_back(rock);
  paper->defeated_by.push_back(scissors.get());

  scissors->wins_against.push_back(paper.get());
  scissors->defeated_by.push_back(rock);

  paper->next = std::move(scissors);
  rock->next = std::move(paper);
}

char victor(const ttt_t::move_t m1, const ttt_t::move_t m2,
            ttt_t::element_t *e) {
  if (m1 == m2)
    return 0; // It's a tie

  if (!e)
    throw std::runtime_error{"Passed nullptr to rps list!"};

  while (e) {
    if (e->type != m1) {
      e = e->next.get();
      continue;
    }

    for (const auto &l : e->wins_against)
      if (l->type == m2)
        return 1; // m1 wins
    for (const auto &l : e->defeated_by)
      if (l->type == m2)
        return 2; // m2 wins
  }

  return -1; // victor unknown, not in database
}
} // namespace cf

namespace {
auto count_element_occurrence(const cf::ttt_t *data) {
  std::unordered_map<cf::ttt_t::move_t, std::size_t> stat{
      {cf::ttt_t::move_t::rock, 0},
      {cf::ttt_t::move_t::paper, 0},
      {cf::ttt_t::move_t::scissors, 0}};

  for (const auto &m : data->recent_moves) {
    switch (m) {
    case cf::ttt_t::move_t::zero:
      break;
    case cf::ttt_t::move_t::rock:
      ++stat.at(cf::ttt_t::move_t::rock);
      break;
    case cf::ttt_t::move_t::paper:
      ++stat.at(cf::ttt_t::move_t::paper);
      break;
    case cf::ttt_t::move_t::scissors:
      ++stat.at(cf::ttt_t::move_t::scissors);
      break;
    default:
      throw std::runtime_error{"Unsupported type found in move history!"};
    }
  }
  return stat;
}

void fill_out_random(auto &c, std::mt19937 &rng) {
  static auto a = std::array{cf::ttt_t::move_t::rock, cf::ttt_t::move_t::paper,
                             cf::ttt_t::move_t::scissors};

  std::uniform_int_distribution<std::size_t> d(0, a.size() - 1);
  for (auto &e : c)
    if (e == cf::ttt_t::move_t::zero)
      e = a[d(rng)];
}
} // namespace

namespace cf {
ttt_t::move_t find_counter_move(const ttt_t::move_t m,
                                const ttt_t::element_t *n) {
  if (!n)
    throw std::runtime_error{"Passed nullptr to rps list!"};

  while (n) {
    if (n->type == m)
      break;
    n = n->next.get();
  }
  return n->defeated_by.front()->type;
}

ttt_t::move_t ai_move(ttt_t *data) {
  auto occmap = count_element_occurrence(data);
  std::vector<ttt_t::move_t> source{};
  source.reserve(data->recent_moves.size());

  auto it = occmap.begin();
  for (decltype(occmap.size()) i = 0; i < occmap.size(); ++i, ++it)
    for (decltype(it->second) j = 0; j < it->second; ++j)
      source.push_back(it->first);

  while (source.size() != data->recent_moves.size())
    source.push_back(ttt_t::move_t::zero);
  fill_out_random(source, data->rng);

  std::uniform_int_distribution<std::size_t> d(0,
                                               data->recent_moves.size() - 1);
  return find_counter_move(source[d(data->rng)], &data->graph);
}

void register_move(ttt_t::move_t m, ttt_t *data) {
  const auto ms = data->recent_moves.size();
  for (auto k = ms - 1; k > 0; --k)
    data->recent_moves[k] = data->recent_moves[k - 1];
  data->recent_moves.front() = m;
}
} // namespace cf
