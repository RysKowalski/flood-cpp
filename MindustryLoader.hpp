#pragma once

#include "Grid.hpp"
#include <cstdint>
#include <string>

class MindustryLoader {
public:
  Grid load_map(const std::string &path);

private:
  inline CellType classify_block(std::int16_t block_id);
  inline void place_block(Grid &grid, std::size_t index, std::int16_t block_id);
};
