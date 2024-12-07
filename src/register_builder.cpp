#include "context.hpp"
#include "evaluate.hpp"
#include <iostream>
#include <pugixml.hpp>

namespace fw {
std::unique_ptr<node_t> number_register(const pugi::xml_node &n,
                                        context_t *ctx) {
  std::string id = n.attribute("id").value();
  double value{};

  try {
    value = evaluate_expression<double>(ctx, n.attribute("value").value());
  } catch (...) {
    std::cerr << "The value: '" << n.attribute("value").value() << "' ";
    std::cerr << "is not a valid number!" << std::endl;
    return {};
  }

  ctx->number_register[id] = value;
  return {};
}

std::unique_ptr<node_t> string_register(const pugi::xml_node &n,
                                        context_t *ctx) {
  std::string id = n.attribute("id").value();
  ctx->string_register[id] =
      evaluate_expression<std::string>(ctx, n.attribute("value").value());
  return {};
}
} // namespace fw
