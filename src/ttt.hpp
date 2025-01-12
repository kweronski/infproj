#pragma once
#include <button.hpp>
#include <context.hpp>
#include <vector>

namespace cf {
struct ttt_t : public ::fw::node_t {
  std::vector<std::vector<char>> board{};
  std::size_t board_size{3};
  char player_move_id{'X'};
  char ai_move_id{'O'};
  // char ai_move_id = 'O'; // to samo co powyzej, starszy standard
  char empty_id{0};

  using position_t = std::pair<std::size_t, std::size_t>; // aliasy
  using win_pattern_t = std::vector<position_t>;
  static std::vector<win_pattern_t> victory_db;
  static bool is_victory(std::vector<std::vector<char>> *, char);

  using block_pattern_t = std::vector<position_t>;
  static std::vector<block_pattern_t> block_db;
  static bool is_need_to_block_move(std::vector<std::vector<char>> *,
                                    char opponent_id,
                                    cf::ttt_t::position_t * = nullptr,
                                    char empty_id = 0);
};

void initialize_ttt(::fw::context_t *);
} // namespace cf
