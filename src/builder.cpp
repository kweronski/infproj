#include "builder.hpp"
#include "context.hpp"
#include <iostream>
#include <optional>
#include <pugixml.hpp>
#include <unordered_map>

namespace fw {
std::unique_ptr<node_t> build_button(const pugi::xml_node &n, context_t *ctx);

std::unique_ptr<node_t> dispatch_builder(const std::string &type,
                                         const pugi::xml_node &details,
                                         context_t *ctx) {
  using nptr_t = std::unique_ptr<node_t>;
  using builder_t = std::function<nptr_t(const pugi::xml_node &, context_t *)>;
  using id_t = std::string;
  using table_t = std::unordered_map<id_t, builder_t>;

  static const table_t disp{{"button", build_button}};

  if (!disp.contains(type))
    return {};
  return disp.at(type)(details, ctx);
}
} // namespace fw

namespace fw {
std::unique_ptr<node_t> build_scene(context_t *ctx, const std::string &source) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(source.c_str());

  if (!result) {
    std::cout << "XML [" << source << "] parsed with errors, attr value: ["
              << doc.child("node").attribute("attr").value() << "]\n";
    std::cout << "Error description: " << result.description() << "\n";
    std::cout << "Error offset: " << result.offset << " (error at [..."
              << (source.c_str() + result.offset) << "]\n\n";
    return {};
  }

  auto rootx = doc.child("root");
  if (!rootx)
    return {};

  std::unique_ptr<node_t> root{new node_t{}};
  for (auto c = rootx.first_child(); c; c = c.next_sibling())
    if (auto ptr = dispatch_builder(c.name(), c, ctx); ptr)
      root->attach(std::move(ptr));
  return root;
}
} // namespace fw
