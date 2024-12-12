#include "scene.hpp"

namespace fw {
unsigned add_routine(scene_t *s, std::function<void(scene_t *)> f) {
  s->routines.push_back({++s->routine_id_, f});
  return s->routine_id_;
}

void remove_routine(scene_t *s, unsigned id) {
  for (auto it = s->routines.begin(); it != s->routines.end(); ++it)
    if (it->first == id) {
      s->routines.erase(it);
      return;
    }
}

void add_command(scene_t *s, std::function<void(scene_t *)> f) {
  s->commands.push_back(f);
}

} // namespace fw
