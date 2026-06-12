#include "MindustryLoader.hpp"
#include "Grid.hpp"
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <zlib.h>


#include <fstream>

std::vector<std::uint8_t> readFile(const std::string filename)
{
    // open the file:
    std::streampos fileSize;
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    std::vector<std::uint8_t> fileData(fileSize);
    file.read((char*) &fileData[0], fileSize);
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

class Reader {
public:
  explicit Reader(std::span<const std::uint8_t> data);

  std::int16_t read_i16() {
    std::int16_t value =
        (static_cast<std::int16_t>(data_[pos_]) << 8) | data_[pos_ + 1];

    pos_ += 2;
    return value;
  }
  std::uint16_t read_u16() {
    std::uint16_t value =
        (static_cast<std::uint16_t>(data_[pos_]) << 8) | data_[pos_ + 1];

    pos_ += 2;
    return value;
  }
  std::uint8_t read_u8() {
    std::uint8_t value = (static_cast<std::uint8_t>(data_[pos_]));

    pos_ += 1;
    return value;
  }
  std::int32_t read_i32() {
    std::int32_t value = (static_cast<std::int32_t>(data_[pos_]) << 24) |
                         (data_[pos_ + 1] << 16) | (data_[pos_ + 2] << 8) |
                         data_[pos_ + 3];

    pos_ += 4;
    return value;
  }

  void skip(std::size_t n) { pos_ += n; };

  void skip_chunk() {
    std::int32_t length = read_i32();
    skip(length);
  }

private:
  std::size_t pos_ = 0;
  std::span<const std::uint8_t> data_;
};

Grid MindustryLoader::load_map(const std::string &path) {

  std::vector<std::uint8_t> raw = readFile(path);
  std::vector<std::uint8_t> data = decompress_zlib(raw);
  std::cout << sizeof(raw) << " " << sizeof(data) << std::endl;

  return Grid(5, 5); }
