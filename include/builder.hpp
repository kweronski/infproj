#pragma once

#include "button.hpp"
#include "resource.hpp"

namespace fw {
std::unique_ptr<node_t> build_scene(const std::string &, const context_t &);
}
