#include "cfusion.hpp"

namespace cf {
void initialize(::fw::context_t *ctx, const char *wdir) {
  fw::cd_to_binary_dir(ctx, wdir);

  fw::load_scene(ctx, "./xml/main_menu.xml", "main_menu");
  fw::activate_scene(ctx, "main_menu");
  create_window_from_registers(ctx);

  /* ADD YOUR CUSTOM SCENE BUILDING FUNCTION HERE */
  initialize_decrypt(ctx);
  initialize_rps(ctx);
}
} // namespace cf
