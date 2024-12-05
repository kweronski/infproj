#include "context.hpp"
#include "builder.hpp"
#include <filesystem>
#include <iostream>

namespace fw {
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

  update_data_t d{};
  d.window = &ctx->window;
  d.last_mouse_pos = &ctx->last_mouse_pos;

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

bool load_scene(context_t *ctx, const std::string &xml, const std::string id) {
  auto scene = build_scene(ctx, xml);
  if (!scene)
    return false;
  if (ctx->scene_map.contains(id))
    return false;
  ctx->scene_map[id] = std::move(scene);
  return true;
}
} // namespace fw
