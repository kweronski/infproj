#pragma once
#include <context.hpp>
#include <button.hpp>

namespace cf {
struct ttt_t : public ::fw::node_t {
  enum class move_t : char { x, o };

  struct element_t {
    std::unique_ptr<element_t> next{};
   // std::list<element_t *> wins_against{};
   // std::list<element_t *> defeated_by{};
   // move_t type{};
  };

 // std::mt19937 rng{std::random_device{}()};
 // std::vector<move_t> recent_moves{};
 // long long unsigned round{1};
//  long long unsigned score{0};
//  long long unsigned aiscore{0};
// element_t graph{};
};
/*
void init_graph(ttt_t::element_t *);

char victor(const ttt_t::move_t m1, const ttt_t::move_t m2,
            ttt_t::element_t *n);

ttt_t::move_t find_counter_move(const ttt_t::move_t m,
                                const ttt_t::element_t *n);

ttt_t::move_t ai_move(ttt_t *);

void register_move(ttt_t::move_t m, ttt_t *);

*/
void initialize_ttt(::fw::context_t *);
} // namespace cf
