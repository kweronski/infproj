#pragma once

#include "context.hpp"

#ifndef EXPRTK_HEADER_INCLUDED
#define EXPRTK_HEADER_INCLUDED
#include "exprtk.hpp"
#endif

namespace fw {
template <typename T>
T evaluate_expression(context_t *ctx, const std::string &expr) {
  static exprtk::parser<double> parser{};
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
  if (parser.compile(parsed, eval))
    value = eval.value();
  else
    throw std::runtime_error{"Error in expression: '" + parsed + "'"};

  return static_cast<T>(value);
}

template <>
std::string evaluate_expression(context_t *ctx, const std::string &);
} // namespace fw
