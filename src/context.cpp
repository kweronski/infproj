#include "context.hpp"
#include "builder.hpp"
#include <filesystem>
#include <iostream>
#include <ranges>

namespace fw {
unsigned add_routine(context_t *ctx, std::function<void(context_t *)> f) {
  ctx->routines.push_back({++ctx->routine_id_, f});
  return ctx->routine_id_;
}

void remove_routine(context_t *ctx, unsigned id) {
  for (auto it = ctx->routines.begin(); it != ctx->routines.end(); ++it)
    if (it->first == id) {
      ctx->routines.erase(it);
      return;
    }
}

void add_command(context_t *ctx, std::function<void(context_t *)> f) {
  ctx->commands.push_back(f);
}

void cd_to_binary_dir(context_t *ctx, const std::string &p) {
  if (!std::filesystem::exists(p))
    return;
  std::filesystem::current_path(
      std::filesystem::absolute(std::filesystem::path{p}.parent_path()));
  ctx->current_working_dir = std::filesystem::current_path().string();
}

void update(context_t *ctx) {
  if (!ctx)
    return;
  while (ctx->window.pollEvent(ctx->event))
    for (const auto &cb : ctx->event_cb_map)
      if (cb.first == ctx->event.type)
        if (cb.second)
          cb.second(ctx);

  if (auto now = std::chrono::steady_clock::now();
      std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                            ctx->frame_start)
          .count() < ctx->frame_time)
    return;
  else
    ctx->frame_start = now;

  auto ipos = sf::Mouse::getPosition(ctx->window);
  ctx->last_mouse_pos = sf::Vector2f{(float)ipos.x, (float)ipos.y};

  update_data_t d{};
  d.window = &ctx->window;
  d.last_mouse_pos = &ctx->last_mouse_pos;

  while (ctx->commands.size()) {
    ctx->commands.front()(ctx);
    ctx->commands.pop_front();
  }

  for (const auto &r : ctx->routines)
    r.second(ctx);

  if (ctx->active_scene)
    ctx->active_scene->update(&d);
}

void render(context_t *ctx) {
  if (!ctx)
    return;
  ctx->window.clear();
  if (ctx->active_scene)
    ctx->window.draw(*ctx->active_scene);
  ctx->window.display();
}

bool activate_scene(context_t *ctx, const std::string &s) {
  if (!ctx)
    return false;
  if (!ctx->scene_map.contains(s))
    return false;
  ctx->active_scene = ctx->scene_map.at(s).get();
  return true;
}

bool load_scene(context_t *ctx, const std::string &xml, std::string id) {
  auto scene = build_scene(ctx, xml);
  if (scene) {
    if (ctx->scene_map.contains(id))
      ctx->scene_map.erase(id);
    ctx->scene_map[id] = std::move(scene);
  }
  return true;
}

template <>
std::optional<std::string> fetch_from_register(const context_t *c,
                                               const std::string &id) {
  if (!c->string_register.contains(id))
    return {};
  return c->string_register.at(id);
}

bool create_window_from_registers(context_t *ctx) {
  if (!ctx->number_register.size() && !ctx->string_register.size())
    return false;
  sf::VideoMode m{640, 480};
  std::string t{};

  if (auto width = fetch_from_register<unsigned>(ctx, "window_width"); width)
    m.width = width.value();
  if (auto height = fetch_from_register<unsigned>(ctx, "window_height"); height)
    m.height = height.value();
  if (auto title = fetch_from_register<std::string>(ctx, "window_title"); title)
    t = title.value();

  ctx->window.create(m, t);
  return true;
}
} // namespace fw
