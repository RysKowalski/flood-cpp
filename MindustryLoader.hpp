#pragma once

#include "Grid.hpp"
#include <cstdint>
#include <span>
#include <string>

class MindustryLoader {
public:
  Grid load_map(const std::string &path);
};

class Reader {
public:
  explicit Reader(std::span<const std::uint8_t> data);

  std::int16_t read_i16();
  std::uint16_t read_u16();
  std::uint8_t read_u8();
  std::int32_t read_i32();

  void skip(std::size_t n);

private:
  std::size_t pos_ = 0;
  std::span<const std::uint8_t> data_;
};
