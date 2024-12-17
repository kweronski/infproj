#pragma once
#include "context.hpp"
#include "rps.hpp"
#include <iostream>

namespace cf {
void initialize(::fw::context_t *, const char *);

void initialize_rps(::fw::context_t *);
} // namespace cf
