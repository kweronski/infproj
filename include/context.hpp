#include "exprtk.hpp"
#include "node.hpp"
#include "resource.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <optional>
#include <ranges>

namespace fw {
struct context_t {
  resource_t<sf::Font> texture{};
  resource_t<sf::Font> font{};

  sf::RenderWindow window{};

  std::chrono::time_point<std::chrono::steady_clock> frame_start{
      std::chrono::steady_clock::now()};
  double frame_time{1000.0 / 60.0};

  node_t *active_scene{};
  std::unordered_map<std::string, std::unique_ptr<node_t>> scene_map{};

  sf::Event event{};
  std::unordered_map<sf::Event::EventType, std::function<void(context_t *)>>
      event_cb_map{};

  sf::Vector2f last_mouse_pos{};
  std::string current_working_dir{};

  std::unordered_map<std::string, double> number_register{};
  std::unordered_map<std::string, std::string> string_register{};

  exprtk::parser<double> parser{};
};

void cd_to_binary_dir(context_t *, const std::string &);
void update(context_t *);
void render(context_t *);
bool activate_scene(context_t *, const std::string &);
bool load_scene(context_t *, const std::string &xml, std::string id);

template <typename T>
std::optional<T> fetch_from_register(const context_t *c,
                                     const std::string &id) {
  if (!c->number_register.contains(id))
    return {};
  return static_cast<T>(c->number_register.at(id));
}

template <>
std::optional<std::string> fetch_from_register(const context_t *c,
                                               const std::string &id);

bool create_window_from_registers(context_t *ctx);

template <typename T>
T evaluate_expression(context_t *ctx, const std::string &expr) {
  auto split = std::ranges::views::split(expr, ' ');
  const std::string numr{"__n:"}; // number identifier
  std::list<std::string> replaced{};
  std::string parsed{};

  for (const auto &f : split) {
    std::string s{f.begin(), f.end()};
    if (s.find(numr) == 0) {
      auto name = s.substr(numr.size(), s.size() - numr.size());
      try {
        auto val = ctx->number_register.at(name);
        replaced.push_back(std::to_string(val));
      } catch (const std::exception &e) {
        std::cerr << "Fetching number at: " << name << " failed: " << e.what()
                  << std::endl;
      }
    } else
      replaced.push_back(s);
  }
  for (const auto &f : replaced)
    parsed += f + " ";
  parsed.pop_back();

  exprtk::expression<double> eval{};
  double value{};
  if (ctx->parser.compile(parsed, eval))
    value = eval.value();
  else
    throw std::runtime_error{"Error in expression: '" + parsed + "'"};

  return static_cast<T>(value);
}

template <>
std::string evaluate_expression(context_t *ctx, const std::string &);
} // namespace fw
