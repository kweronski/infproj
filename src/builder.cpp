#include "builder.hpp"
#include "context.hpp"
#include "evaluate.hpp"
#include <iostream>

namespace {
std::unique_ptr<pugi::xml_document> open_xml_file(const std::string &f) {
  auto doc = std::make_unique<pugi::xml_document>();
  auto result = doc->load_file(f.c_str());

  if (!result) {
    std::cerr << "XML [" << f << "] parsed with errors, attr value: ["
              << doc->child("node").attribute("attr").value() << "]\n";
    std::cerr << "Error description: " << result.description() << "\n";
    std::cerr << "Error offset: " << result.offset << " (error at [..."
              << (f.c_str() + result.offset) << "]\n\n";
    return {};
  }

  return doc;
}

void write_to_register(const pugi::xml_node &n, fw::scene_t *s,
                       fw::register_t<std::string, double> *reg,
                       bool overwrite = true) {
  if (std::string{n.name()} != "nreg")
    return;

  if (!n.attribute("id"))
    throw std::runtime_error{"A register entry must have an ID!"};
  if (!n.attribute("value"))
    throw std::runtime_error{"A register entry must have a value!"};

  auto evaluated = evaluate<double>(s, n.attribute("value").value());
  reg->add(n.attribute("id").value(), evaluated, overwrite);
  std::cout << "\t" << "added {" << n.attribute("id").value() << ", "
            << evaluated << "} to number register" << std::endl;
}

void write_to_register(const pugi::xml_node &n, fw::scene_t *s,
                       fw::register_t<std::string, std::string> *reg,
                       bool overwrite = true) {
  if (std::string{n.name()} != "sreg")
    return;

  if (!n.attribute("id"))
    throw std::runtime_error{"A register entry must have an ID!"};
  if (!n.attribute("value"))
    throw std::runtime_error{"A register entry must have a value!"};

  auto evaluated = evaluate<std::string>(s, n.attribute("value").value());
  reg->add(n.attribute("id").value(), evaluated, overwrite);
  std::cout << "\t" << "added {" << n.attribute("id").value() << ", "
            << evaluated << "} to string register" << std::endl;
}

std::unique_ptr<fw::node_t> build_register(const pugi::xml_node &n,
                                           fw::context_t *, fw::scene_t *s) {
  write_to_register(n, s, &s->local_registers.string);
  write_to_register(n, s, &s->local_registers.number);
  return {};
}

template <typename D>
void update_global_registers(fw::context_t *ctx, fw::scene_t *s, const D &doc) {
  for (auto c = doc->first_child(); c; c = c.next_sibling()) {
    write_to_register(c, s, &ctx->global_registers.number);
    write_to_register(c, s, &ctx->global_registers.string);
  }
}

void load_scene(fw::context_t *ctx, const pugi::xml_node &n) {
  if (auto name = std::string{n.name()}; name != "scene")
    return;

  std::string src{}, id{};

  if (auto i = n.attribute("id"); !i)
    throw std::runtime_error{"Loading a scene failed, as no id was specified!"};
  else
    id = std::string{i.value()};

  if (auto p = n.attribute("src"); !p)
    throw std::runtime_error{
        "Loading a scene failed, as no src was specified!"};
  else
    src = std::string{p.value()};

  load_scene(ctx, src, id, false);
}

void load_font(fw::scene_t *s, const pugi::xml_node &n) {
  if (auto name = std::string{n.name()}; name != "font")
    return;

  std::string src{}, id{};

  if (auto i = n.attribute("id"); !i)
    throw std::runtime_error{"Loading a font failed, as no id was specified!"};
  else
    id = std::string{i.value()};

  if (auto p = n.attribute("src"); !p)
    throw std::runtime_error{"Loading a font failed, as no src was specified!"};
  else
    src = std::string{p.value()};

  s->font.load(id, src);
  std::cout << "\t" << "loaded font: " << id << ", from: " << src << std::endl;
}

void load_texture(fw::scene_t *s, const pugi::xml_node &n) {
  if (auto name = std::string{n.name()}; name != "texture")
    return;

  std::string src{}, id{};

  if (auto i = n.attribute("id"); !i)
    throw std::runtime_error{
        "Loading a texture failed, as no id was specified!"};
  else
    id = std::string{i.value()};

  if (auto p = n.attribute("src"); !p)
    throw std::runtime_error{
        "Loading a texture failed, as no src was specified!"};
  else
    src = std::string{p.value()};

  s->texture.load(id, src);
  std::cout << "\t" << "loaded texture: " << id << ", from: " << src
            << std::endl;
}

template <typename D>
std::list<std::function<void()>>
load_scene_resources(fw::context_t *ctx, fw::scene_t *s, const D &doc) {
  std::list<std::function<void()>> scenes{};
  for (auto c = doc->first_child(); c; c = c.next_sibling()) {
    load_texture(s, c);
    scenes.push_back([=]() { load_scene(ctx, c); });
    load_font(s, c);
  }
  return scenes;
}
} // namespace

namespace fw {
std::unique_ptr<node_t> build_button(const pugi::xml_node &, context_t *,
                                     scene_t *);

std::unique_ptr<node_t> dispatch_builder(const std::string &type,
                                         const pugi::xml_node &details,
                                         context_t *c, scene_t *s) {
  using nptr_t = std::unique_ptr<node_t>;
  using builder_t =
      std::function<nptr_t(const pugi::xml_node &, context_t *, scene_t *)>;

  using table_t = std::unordered_map<std::string, builder_t>;
  static const table_t disp{{"button", build_button},
                            {"nreg", build_register},
                            {"sreg", build_register}};

  if (!disp.contains(type))
    return {};
  return disp.at(type)(details, c, s);
}

void build_scene(context_t *ctx, const std::string &source,
                 const std::string &id) {
  std::cout << "BUILDER: building scene: " << id << ", from: " << source
            << std::endl;
  auto doc = open_xml_file(source);
  if (!doc)
    throw std::runtime_error{"Failed to open xml file: '" + source + "'"};

  auto s = create_scene_skeleton(ctx);

  std::cout << "BUILDER: updating global registers..." << std::endl;
  update_global_registers(ctx, &s, doc);

  std::cout << "BUILDER: loading scene resources..." << std::endl;
  auto scenes = load_scene_resources(ctx, &s, doc);

  auto root = doc->child("root");
  if (!root)
    return;

  std::cout << "BUILDER: building scene graph..." << std::endl;
  for (auto c = root.first_child(); c; c = c.next_sibling()) {
    if (auto ptr = dispatch_builder(c.name(), c, ctx, &s); ptr) {
      std::cout << "\tadding node type: '" << c.name() << "'";
      if (auto id = std::string{c.attribute("id").value()}; id.size()) {
        std::cout << ", as VIP: '" << id << "'";
        s.vip_nodes.emplace(id, ptr.get());
      }
      std::cout << std::endl;
      s.root->attach(std::move(ptr));
    }
  }

  std::cout << "BUILDER: building complete.\n" << std::endl;
  ctx->scene_map.emplace(id, std::move(s));

  for (const auto &f : scenes)
    f();
}
} // namespace fw
