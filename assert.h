#pragma once

#include "debug.h"

#define assert(result) if(!result) {ABORT_MSG("assert failed")}

