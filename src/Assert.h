#pragma once

#include "Log.h"

#define BL_ASSERT(condition, ...) if (!(condition)) { Log::logError(__VA_ARGS__); }