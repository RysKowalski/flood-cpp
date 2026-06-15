#pragma once

#include "BinaryReader.hpp"
#include "Grid.hpp"
#include <cstdint>
#include <string>

class MindustryLoader {
public:
  MindustryLoader(const std::string &path);
  Grid load_map();

private:
  Reader r;
  std::vector<std::uint8_t> read_file(const std::string &path);
  Grid read_map_chunk();
  void skip_terrain(int total_tiles);
  void read_blocks(Grid &grid);
  inline CellType classify_block(std::int16_t block_id);
  inline void place_block(Grid &grid, std::size_t index, std::int16_t block_id);
};
