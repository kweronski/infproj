#ifndef GOMOKU_HPP
#define GOMOKU_HPP

#include <SFML/Graphics.hpp>
#include <context.hpp>
#include <button.hpp>

struct gomoku_root_t : public ::fw::node_t {
  int ownership[15*15]{};
  int player = 1;
  int winner = 0;
  int move = 0;
  int wins[2] = {0, 0};
  std::chrono::time_point<std::chrono::system_clock> reset_time;
};

void initialize_gomoku(fw::context_t *ctx);
#endif
