#pragma once

#include "exprtk.hpp"
#include "register.hpp"
#include "scene.hpp"
#include <concepts>
#include <iostream>
#include <ranges>

namespace fw {
// This function replaces variable names with their values
// that are stored within a register
template <typename T>
std::string replace(const scene_t *scene, const std::string &keyword,
                    const std::string &input) {

  auto split = std::ranges::views::split(input, ' ');
  std::list<std::string> replaced{};
  std::string parsed{};

  for (const auto &f : split) {
    std::string s{f.begin(), f.end()};
    if (s.find(keyword) == 0) {
      auto name = s.substr(keyword.size(), s.size() - keyword.size());
      T val{};

      try {
        val = get_value_from_register<T>(scene, name);
      } catch (const std::exception &e) {
        throw std::runtime_error{"Trying to replace '" + name +
                                 "' with its value failed!"};
      }

      if constexpr (std::same_as<std::string, T>)
        replaced.push_back(val);
      else
        replaced.push_back(std::to_string(val));
    } else
      replaced.push_back(s);
  }

  for (const auto &f : replaced)
    parsed += f + " ";
  parsed.pop_back();

  return parsed;
}

template <typename T> T evaluate(const scene_t *s, const std::string &input) {
  static exprtk::parser<double> parser{};
  exprtk::expression<double> eval{};

  if (auto str = replace<T>(s, "__n:", input); !parser.compile(str, eval))
    throw std::runtime_error{"Error in expression: '" + str + "'"};

  return static_cast<T>(eval.value());
}

template <>
inline std::string evaluate(const scene_t *s, const std::string &input) {
  return replace<std::string>(s, "__s:", input);
}
} // namespace fw
