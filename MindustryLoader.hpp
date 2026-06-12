#pragma once

#include "Grid.hpp"
#include <string>

class MindustryLoader {
public:
  Grid load_map(const std::string &path);
};
