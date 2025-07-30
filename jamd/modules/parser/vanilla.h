#pragma once

#include "../../ext/alias.h"

const static Regex vanilla_regex(
    R"(\[(\d+):(\d+):(\d+)\])"    // hour, min, sec
    R"(\[([^]]+)/([^]/]+)\])"      // thread, logging
    R"(: (.*))",                   // content
    OptimizeRegex
);

