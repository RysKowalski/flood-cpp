#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class Reader {
public:
  explicit Reader(std::vector<std::uint8_t> data);

  std::int16_t read_i16();
  std::uint16_t read_u16();
  std::uint8_t read_u8();
  bool read_bool();
  std::int32_t read_i32();

  void skip(std::size_t n);
  void skip_chunk();

private:
  std::size_t pos_ = 0;
  std::vector<std::uint8_t> data_;
};
