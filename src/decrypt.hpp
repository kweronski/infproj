#pragma once
#include "button.hpp"
#include "context.hpp"
#include <algorithm>
#include <cctype>
#include <random>

namespace cf {
class key_t : public fw::button_t<sf::RectangleShape> {
public:
  key_t(char k, char v) : k_{k}, v_{v}, t_{} {}

  void update_string() {
    this->label()->setString(std::string{k_} + std::string{"="} +
                             std::string{v_});
    this->center();
  }

  char key() const { return k_; }
  void key(char k) {
    k_ = k;
    update_string();
  }

  char value() const { return v_; }
  void value(char v) {
    v_ = v;
    update_string();
  }

  float time() const { return t_; }
  void time(float t) { t_ = t; }

private:
  char k_, v_;
  float t_;
};

struct decrypt_t : public fw::node_t {
  std::string key{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  std::mt19937 rng{std::random_device{}()};
  unsigned health{}, time_left{};
  std::string plaintext{}, encrypted{};
};

void initialize_decrypt(fw::context_t *ctx);

void generate_key(decrypt_t *p);

void initialize_round(fw::scene_t *, fw::context_t *);

inline std::string encrypt(const std::string &key, const std::string msg) {
  const std::string alph{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  std::string enc{};
  for (std::size_t i = 0; i < msg.size(); ++i)
    for (std::size_t j = 0; j < alph.size(); ++j)
      if (msg[i] == alph[j] || msg[i] == std::tolower(alph[j]))
        enc += key[j];
  return enc;
}

std::string pick_word();
} // namespace cf
