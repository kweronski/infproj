#pragma once
#include "context.hpp"
#include <algorithm>
#include <array>
#include <list>
#include <memory>
#include <random>

namespace cf {
struct rps_t : public ::fw::node_t {
  enum class move_t : char { zero, rock, paper, scissors };

  struct element_t {
    std::unique_ptr<element_t> next{};
    std::list<element_t *> wins_against{};
    std::list<element_t *> defeated_by{};
    move_t type{};
  };

  std::mt19937 rng{std::random_device{}()};
  std::array<move_t, 20> recent_moves{};
  long long unsigned round{1};
  long long unsigned score{0};
  long long unsigned aiscore{0};
  element_t graph{};
};

void init_graph(rps_t::element_t *);

char victor(const rps_t::move_t m1, const rps_t::move_t m2,
            rps_t::element_t *n);

rps_t::move_t find_counter_move(const rps_t::move_t m,
                                const rps_t::element_t *n);

rps_t::move_t ai_move(rps_t *);

void register_move(rps_t::move_t m, rps_t *);

void initialize_rps(::fw::context_t *);
} // namespace cf
