#include "evaluate.hpp"

namespace fw {
template <>
std::string evaluate_expression(context_t *ctx, const std::string &expr) {
  auto split = std::ranges::views::split(expr, ' ');
  const std::string strr{"__s:"}; // string identifier
  std::list<std::string> replaced{};
  std::string parsed{};

  for (const auto &f : split) {
    std::string s{f.begin(), f.end()};
    if (s.find(strr) == 0) {
      auto name = s.substr(strr.size(), s.size() - strr.size());
      try {
        auto val = ctx->string_register.at(name);
        replaced.push_back(val);
      } catch (const std::exception &e) {
        std::cerr << "Fetching string at: " << name << " failed: " << e.what()
                  << std::endl;
      }
    } else
      replaced.push_back(s);
  }
  for (const auto &f : replaced)
    parsed += f + " ";
  parsed.pop_back();
  return parsed;
}
} // namespace fw
