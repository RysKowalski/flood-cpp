#include "MindustryLoader.hpp"
#include "Grid.hpp"

std::uint16_t Reader::read_u16() {
  std::uint16_t value =
      (static_cast<std::uint16_t>(data_[pos_]) << 8) | data_[pos_ + 1];

  pos_ += 2;
  return value;
};

Grid MindustryLoader::load_map(const std::string &path) { return Grid(5, 5); }
