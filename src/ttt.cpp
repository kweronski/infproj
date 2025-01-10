#include "ttt.hpp"
#include <random>
#include <thread>
#include <unordered_map>

namespace {
struct ui_button_t {
  std::string text_label;
  std::string text_font_id;

  float position_x{};
  float position_y{};
  float size_w{};
  float size_h{};
  float text_size{};
  float outline_thickness{};

  sf::Color button_default_fill_color{sf::Color::White};
  sf::Color button_hover_fill_color{sf::Color::White};

  sf::Color button_default_outline_color{sf::Color::White};
  sf::Color button_hover_outline_color{sf::Color::Red};

  sf::Color text_default_fill_color{sf::Color::Black};
  sf::Color text_hover_fill_color{sf::Color::White};
};

class make_random_number {
public:
  static std::size_t from_closed_range(std::size_t range_begin,
                                       std::size_t range_end) {
    std::uniform_int_distribution<std::size_t> range{
        range_begin,
        range_end}; // przedzial liczb lacznie z a i b, przedział zamkniety
    return range(twister);
  }

private:
  static inline std::mt19937 twister{std::random_device{}()};
};

fw::button_t<sf::RectangleShape> *create_ui_button(fw::scene_t *s,
                                                   ui_button_t *);
void create_ui(fw::context_t *ctx, fw::scene_t *);
fw::button_t<sf::RectangleShape> *create_back_button(fw::context_t *ctx,
                                                     fw::scene_t *);
cf::ttt_t *initialize_ttt_data(fw::scene_t *s);
void update_ttt(cf::ttt_t *tttptr, std::size_t i, std::size_t j, fw::scene_t *s,
                fw::button_t<sf::RectangleShape> *b);
void reset_ttt(fw::context_t *ctx, fw::scene_t *s);
void create_board_ui(fw::scene_t *s);

std::pair<std::size_t, std::size_t>
generate_ai_position(std::size_t board_size);
bool is_position_available(
    std::vector<std::vector<char>> *, std::size_t, std::size_t,
    char = 0); // funkcja zostanie autoamtycznie wybrana ze wzgledu na typ
bool is_position_available(std::vector<std::vector<char>> *board,
                           std::pair<std::size_t, std::size_t> position,
                           char empty_id = 0) {
  return is_position_available(board, position.first, position.second,
                               empty_id);
}

bool is_board_full(std::vector<std::vector<char>> *, char empty_id = 0);
std::pair<std::size_t, std::size_t>
pick_ai_position(std::vector<std::vector<char>> *, char empty_id = 0);
void make_ai_move(fw::scene_t *s);

void create_game_over_menu(fw::scene_t *s, std::string message);

} // namespace

namespace cf {
void initialize_ttt(fw::context_t *ctx) {
  const std::string scene_id{"ttt"};
  ctx->scene_map.emplace(scene_id, fw::create_scene_skeleton(ctx));
  auto s = &ctx->scene_map.at(scene_id);

  s->font.load("Digital7", "./font/digital-7.ttf");
  s->font.load("AnonymousPro", "./font/AnonymousPro-Regular.ttf");

  reset_ttt(ctx, s);
}
} // namespace cf

namespace {
fw::button_t<sf::RectangleShape> *create_ui_button(fw::scene_t *s,
                                                   ui_button_t *data) {
  auto ptr = new fw::button_t<sf::RectangleShape>{};
  s->root->attach(
      std::unique_ptr<fw::node_t>{ptr}); // zamiast (*(*s).root).attach

  ptr->shape()->setSize({data->size_w, data->size_h});
  ptr->shape()->setFillColor(data->button_default_fill_color);
  ptr->shape()->setOutlineThickness(data->outline_thickness);
  ptr->shape()->setOutlineColor(data->button_default_outline_color);
  ptr->shape()->setPosition({data->position_x, data->position_y});
  ptr->label()->setFont(s->font.get(data->text_font_id));
  ptr->label()->setCharacterSize(data->text_size);
  ptr->label()->setString(data->text_label);
  ptr->center();

  ptr->add_hover_cb(
      [fill_color = data->button_hover_fill_color,
       outline_color = data->button_hover_outline_color](auto *b) {
        // std::cout<<"hover"<<std::endl;
        b->shape()->setOutlineColor(outline_color);
        b->shape()->setFillColor(fill_color);
      });

  ptr->add_unhover_cb(
      [fill_color = data->button_default_fill_color,
       outline_color = data->button_default_outline_color](auto *b) {
        b->shape()->setOutlineColor(outline_color);
        b->shape()->setFillColor(fill_color);
      });

  return ptr;
}

void create_ui(fw::context_t *ctx, fw::scene_t *s) {
  create_back_button(ctx, s);
  create_board_ui(s);
}

fw::button_t<sf::RectangleShape> *create_back_button(fw::context_t *ctx,
                                                     fw::scene_t *s) {
  // auto wh = fw::get_value_from_register<float>(s, "window_height");
  auto ww = fw::get_value_from_register<float>(s, "window_width");

  ui_button_t d{};
  d.text_label = "Back";
  d.text_font_id = "AnonymousPro";
  d.text_size = 40;

  d.size_w = 100;
  d.size_h = 50;

  d.outline_thickness = 5;
  d.position_x = ww - d.size_w - d.outline_thickness * 2;
  d.position_y = 0 + d.size_h + d.outline_thickness * 2;

  d.button_default_fill_color = sf::Color::Black;
  d.button_hover_fill_color = sf::Color::Black;

  d.button_default_outline_color = sf::Color::White;
  d.button_hover_outline_color = sf::Color::Red;

  d.text_default_fill_color = sf::Color::White;
  d.text_hover_fill_color = sf::Color::White;

  auto ptr = create_ui_button(s, &d); // ustalic pozycje w X

  ptr->add_click_cb([ctx, s](auto *) {
    fw::add_command(s, [ctx](auto *sc) { reset_ttt(ctx, sc); });
    ctx->active_scene = &ctx->scene_map.at("selection_menu");
  });
  return ptr;
}

cf::ttt_t *initialize_ttt_data(fw::scene_t *s) {
  auto tttptr = new cf::ttt_t{};
  s->root = std::unique_ptr<::fw::node_t>{tttptr};

  tttptr->board.resize(tttptr->board_size);

  for (std::size_t i = 0; i < tttptr->board.size(); i++) {
    tttptr->board[i].resize(tttptr->board_size);
  }

  for (std::size_t i = 0; i < tttptr->board.size(); i++)
    for (std::size_t j = 0; j < tttptr->board[i].size(); j++)
      tttptr->board[i][j] = 0;

  return tttptr;
}

void create_board_ui(fw::scene_t *s) {
  auto wh = fw::get_value_from_register<float>(s, "window_height");
  auto ww = fw::get_value_from_register<float>(s, "window_width");
  auto tttptr = dynamic_cast<cf::ttt_t *>(s->root.get());

  const float sz = 300, padding = 10;
  const float side =
      sz * tttptr->board_size + padding * 2 * (tttptr->board_size - 1);

  const float initial_x = ww / 2 - side / 2;
  const float initial_y = (wh - side) / 2;

  ui_button_t d{};
  d.text_label = " ";
  d.text_font_id = "AnonymousPro";
  d.text_size = 40;

  d.size_w = sz;
  d.size_h = d.size_w;
  d.position_x = initial_x;
  d.position_y = initial_y;

  d.outline_thickness = 5;

  d.button_default_fill_color = sf::Color::Black;
  d.button_hover_fill_color = sf::Color::Black;

  d.button_default_outline_color = sf::Color::White;
  d.button_hover_outline_color = sf::Color::Red;

  d.text_default_fill_color = sf::Color::White;
  d.text_hover_fill_color = sf::Color::White;

  for (std::size_t i = 0; i < tttptr->board.size(); i++) {
    for (std::size_t j = 0; j < tttptr->board[i].size(); j++) {
      auto panel = create_ui_button(s, &d);
      s->vip_nodes.emplace(
          "panel" + std::to_string(i) + "x" + std::to_string(j), panel);
      panel->add_click_cb(
          [tttptr, i, j, s](auto *b) { update_ttt(tttptr, i, j, s, b); });
      d.position_x += padding * 2 + sz;
    }
    d.position_x = initial_x;
    d.position_y += padding * 2 + sz;
  }
}

void update_ttt(cf::ttt_t *tttptr, std::size_t i, std::size_t j, fw::scene_t *s,
                fw::button_t<sf::RectangleShape> *b) {
  if (!is_position_available(&tttptr->board, i, j, tttptr->empty_id))
    return;
  tttptr->board[i][j] = tttptr->player_move_id;
  b->label()->setString(tttptr->player_move_id);
  b->center();
  if (cf::ttt_t::is_victory(&tttptr->board, tttptr->player_move_id))
    create_game_over_menu(s, "You win!");
  else if (is_board_full(&tttptr->board, tttptr->empty_id))
    create_game_over_menu(s, "It's a tie!");
  else {
    make_ai_move(s);
    if (cf::ttt_t::is_victory(&tttptr->board, tttptr->ai_move_id))
      create_game_over_menu(s, "You loose!");
    else if (is_board_full(&tttptr->board, tttptr->empty_id))
      create_game_over_menu(s, "It's a tie!");
  }
}

inline std::pair<std::size_t, std::size_t>
generate_ai_position(std::size_t board_size) {
  const std::size_t range_begin{0}, range_end{board_size - 1};
  return {make_random_number::from_closed_range(range_begin, range_end),
          make_random_number::from_closed_range(range_begin, range_end)};
}

inline bool is_position_available(std::vector<std::vector<char>> *board,
                                  std::size_t x, std::size_t y, char empty_id) {
  return (*board)[x][y] == empty_id;
}

bool is_board_full(std::vector<std::vector<char>> *board, char empty_id) {
  for (std::size_t i = 0; i < board->size(); ++i)
    for (std::size_t j = 0; j < (*board)[i].size(); ++j)
      if (is_position_available(board, i, j, empty_id))
        return false;
  return true;
}

std::pair<std::size_t, std::size_t>
pick_ai_position(std::vector<std::vector<char>> *board, char empty_id) {
  if (is_board_full(board, empty_id))
    throw std::runtime_error{"pick_ai_position: TicTacToe board is full!"};

  auto position = generate_ai_position(board->size());
  while (!is_position_available(board, position, empty_id))
    position = generate_ai_position(board->size());
  return position;
}

void make_ai_move(fw::scene_t *s) {
  auto tttptr = dynamic_cast<cf::ttt_t *>(s->root.get()); // wylowienie typu
  auto p{pick_ai_position(&tttptr->board, tttptr->empty_id)};

  if (is_position_available(&tttptr->board, {1, 1}, tttptr->empty_id))
    p = {1, 1};
  tttptr->board[p.first][p.second] = tttptr->ai_move_id;
  const std::string panel_id =
      "panel" + std::to_string(p.first) + "x" + std::to_string(p.second);
  auto panel = dynamic_cast<fw::button_t<sf::RectangleShape> *>(
      s->vip_nodes.at(panel_id)); // wylowienie typu
  panel->label()->setString(tttptr->ai_move_id);
  panel->center();
}

bool is_pattern_present(std::vector<std::vector<char>> *board,
                        const cf::ttt_t::win_pattern_t *pattern, char id) {
  for (const auto &pos : *pattern)
    if ((*board)[pos.first][pos.second] != id)
      return false;
  return true;
}

void reset_ttt(fw::context_t *ctx, fw::scene_t *s) {
  fw::clear_routines(s);
  if (s->vip_nodes.size())
    s->vip_nodes.clear();
  initialize_ttt_data(s);
  create_ui(ctx, s);
  fw::add_routine(s, [](auto *ptr) {
    if (ptr->global_registers->number.template get_value<bool>(
            "ttt_update_settings")) {
      ptr->global_registers->number.add("ttt_update_settings", 0);
      auto player_type =
          fw::get_value_from_register<bool>(ptr, "ttt_player_type");
      auto tttptr =
          dynamic_cast<cf::ttt_t *>(ptr->root.get()); // wylowienie typu
      if (player_type == 0) {
        tttptr->player_move_id = 'O';
        tttptr->ai_move_id = 'X';
      } else {
        tttptr->player_move_id = 'X';
        tttptr->ai_move_id = 'O';
      }
    }
  });
}

void create_game_over_menu(fw::scene_t *s, std::string message) {
  for (const auto &pair : s->vip_nodes) {
    pair.second->deactivate();
    pair.second->hide();
  }
  auto wh = fw::get_value_from_register<float>(s, "window_height");
  auto ww = fw::get_value_from_register<float>(s, "window_width");
  ui_button_t d{};
  d.text_label = message;
  d.text_font_id = "AnonymousPro";
  d.text_size = 100;

  d.size_w = 600;
  d.size_h = 500;

  d.outline_thickness = 5;
  d.position_x = ww / 2 - d.size_w / 2 - d.outline_thickness;
  d.position_y = wh / 2 - d.size_h / 2 - d.outline_thickness;

  d.button_default_fill_color = sf::Color::Black;
  d.button_hover_fill_color = sf::Color::Black;

  d.button_default_outline_color = sf::Color::White;
  d.button_hover_outline_color = sf::Color::Red;

  d.text_default_fill_color = sf::Color::White;
  d.text_hover_fill_color = sf::Color::White;
  create_ui_button(s, &d);
}
} // namespace

bool cf::ttt_t::is_victory(std::vector<std::vector<char>> *board, char id) {
  for (const auto &pat :
       victory_db) //  for(std::size_t i = 0; i < victory_db.size(); ++i) ->>>>
                   //  pat = victory_db[i]
    if (is_pattern_present(board, &pat, id))
      return true;
  return false;
}

std::vector<cf::ttt_t::win_pattern_t> cf::ttt_t::victory_db{
    // wiersze
    {{0, 0}, {0, 1}, {0, 2}},
    {{1, 0}, {1, 1}, {1, 2}},
    {{2, 0}, {2, 1}, {2, 2}},
    // kolumny
    {{0, 0}, {1, 0}, {2, 0}},
    {{0, 1}, {1, 1}, {2, 1}},
    {{0, 2}, {1, 2}, {2, 2}},
    // skos1
    {{0, 0}, {1, 1}, {2, 2}},
    // skos2
    {{0, 2}, {1, 1}, {2, 0}}};
