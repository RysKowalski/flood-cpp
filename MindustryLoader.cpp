#include "MindustryLoader.hpp"
#include "BinaryReader.hpp"
#include "Grid.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <zlib.h>

std::vector<std::uint8_t> readFile(const std::string filename) {
  // open the file:
  std::streampos fileSize;
  std::ifstream file(filename, std::ios::binary);

  // get its size:
  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  // read the data:
  std::vector<std::uint8_t> fileData(fileSize);
  file.read((char *)&fileData[0], fileSize);
  return fileData;
}

std::vector<std::uint8_t>
decompress_zlib(const std::vector<std::uint8_t> &compressed) {
  if (compressed.empty()) {
    return {};
  }

  uLongf output_size = compressed.size() * 4;
  std::vector<std::uint8_t> output(output_size);

  while (true) {
    const int result =
        uncompress(output.data(), &output_size, compressed.data(),
                   static_cast<uLong>(compressed.size()));

    if (result == Z_OK) {
      output.resize(output_size);
      return output;
    }

    if (result == Z_BUF_ERROR) {
      output_size *= 2;
      output.resize(output_size);
      continue;
    }

    throw std::runtime_error("zlib decompression failed");
  }
}

MindustryLoader::MindustryLoader(const std::string &path)
    : r(decompress_zlib(readFile(path))) {};

std::vector<std::uint8_t> MindustryLoader::read_file(const std::string &path) {
  return decompress_zlib(readFile(path));
}

Grid MindustryLoader::load_map() {
  std::cout << "HEADER: ";
  std::cout << static_cast<char>(r.read_u8());
  std::cout << static_cast<char>(r.read_u8());
  std::cout << static_cast<char>(r.read_u8());
  std::cout << static_cast<char>(r.read_u8());
  std::cout << '\n';

  std::cout << "VERSION: ";
  std::cout << r.read_i32() << "\n";

  r.skip_chunk(); // meta
  r.skip_chunk(); // content
  r.skip_chunk(); // IDK

  Grid grid = read_map_chunk();
  int total_tiles = grid.width() * grid.height();

  skip_terrain(total_tiles);

  read_blocks(grid);

  return grid;
}

Grid MindustryLoader::read_map_chunk() {
  std::int32_t map_size = r.read_i32();

  std::uint16_t width = r.read_u16();
  std::uint16_t height = r.read_u16();
  std::cout << "width: " << width << " height: " << height << "\n";

  return Grid(width, height);
}

void MindustryLoader::skip_terrain(int total_tiles) {
  int terrain_files = 0;
  while (terrain_files < total_tiles) {

    std::int16_t block_id = r.read_i16();
    std::int16_t overlay_id = r.read_i16();
    std::uint8_t run = r.read_u8();

    terrain_files += run + 1;
  }
}

void MindustryLoader::read_blocks(Grid &grid) {
  int total_tiles = grid.width() * grid.height();

  int tile_index = 0;
  while (tile_index < total_tiles) {

    std::int16_t block_id = r.read_i16();
    std::uint8_t flags = r.read_u8();

    bool has_entity = (flags & 1) != 0;
    bool has_data = (flags & 4) != 0;

    // SIMPLE RLE BLOCK
    if (not has_entity and not has_data) {
      std::uint8_t run = r.read_u8();
      for (int i = 0; i < run + 1; i++) {
        place_block(grid, tile_index, block_id);
        tile_index++;
      }
      continue;
    }

    // OPTIONAL TILE DATA
    if (has_data)
      r.skip(7);

    // OPTIONAL ENTITY DATA
    if (has_entity) {
      bool is_center = r.read_bool();
      if (is_center)
        r.skip_chunk();
    }

    place_block(grid, tile_index, block_id);
    tile_index++;
  }
}

inline CellType MindustryLoader::classify_block(std::int16_t block_id) {
  switch (block_id) {
  // WALL blocks (your full range)
  case 80:
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
  case 86:
  case 87:
  case 88:
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
  case 102:
  case 103:
  case 104:
  case 124:
  case 125:
  case 156:
  case 157:
  case 158:
  case 160:
  case 178:
    return CellType::WALL;

  // GENERATORS
  case 339:
  case 340:
  case 341:
  case 342:
  case 343:
  case 344:
    return CellType::GENERATOR;

  default:
    return CellType::NOTHING;
  }
}

inline void MindustryLoader::place_block(Grid &grid, std::size_t index,
                                         std::int16_t block_id) {
  const std::size_t width = grid.width();
  const std::size_t height = grid.height();

  const std::size_t x = index % width;
  const std::size_t y = index / width;

  const std::size_t flipped_y = height - 1 - y;

  Cell &cell = grid.at(x, flipped_y);

  const CellType type = classify_block(block_id);

  cell.type = type;

  if (type == CellType::GENERATOR) {
    cell.value = 0.0;
  }
}
