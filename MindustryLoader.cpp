#include "MindustryLoader.hpp"
#include "Grid.hpp"

std::int16_t Reader::read_i16() {
  std::int16_t value =
      (static_cast<std::int16_t>(data_[pos_]) << 8) | data_[pos_ + 1];

  pos_ += 2;
  return value;

}

std::uint16_t Reader::read_u16() {
  std::uint16_t value =
      (static_cast<std::uint16_t>(data_[pos_]) << 8) | data_[pos_ + 1];

  pos_ += 2;
  return value;
};

std::uint8_t Reader::read_u8() {
  std::uint8_t value =
      (static_cast<std::uint8_t>(data_[pos_]));

  pos_ += 1;
  return value;
};

std::int32_t Reader::read_i32() {
  std::int32_t value =
      (static_cast<std::int32_t>(data_[pos_]) << 24) | (data_[pos_ + 1] << 16) | (data_[pos_ + 2] << 8) | data_[pos_ + 3];

  pos_ += 4;
  return value;
}

void Reader::skip(std::size_t n) {
  pos_ += n;
}

Grid MindustryLoader::load_map(const std::string &path) { return Grid(5, 5); }
