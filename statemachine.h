#pragma once

#include "meta-settings.h"

callback_t state_include(std::ifstream& f);
callback_t guess_state(std::ifstream& f);
void clear_empty(std::ifstream& f);
callback_t guess_class_state(std::ifstream& f);
