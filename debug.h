#pragma once

#include "vga.h"

#define ABORT_MSG(...)         \
    vga_printf(__VA_ARGS__); \
    abort();

void abort();
