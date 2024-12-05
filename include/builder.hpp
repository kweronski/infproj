#pragma once

#include "button.hpp"

namespace fw {
struct context_t;
std::unique_ptr<node_t> build_scene(const context_t &, const std::string &);
}
