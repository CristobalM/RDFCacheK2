//
// Created by cristobal on 21-11-21.
//

#ifndef ULHEADERIOHANDLER_CUSTOM_SERIALIZATION_HPP
#define ULHEADERIOHANDLER_CUSTOM_SERIALIZATION_HPP

#include "serialization_util.hpp"
#include <fstream>

namespace k2cache {

class ULHeaderIOHandlerCustomSerialization {
public:
  class Reader {
    std::ifstream &is;
    uint64_t counter;

    uint64_t sz;

  public:
    explicit Reader(std::ifstream &is) : is(is), counter(0), sz(0) {
      sz = read_u64(is);
    }

    template <typename T> bool read_value(T &out) {
      T::read_value(is, out);
      counter++;
      return counter <= sz;
    }
  };

  class Writer {
    std::ofstream &ofs;
    uint64_t elements_in_file;
    long header_pos;

  public:
    Writer(std::ofstream &ofs, uint64_t elements_in_file)
        : ofs(ofs), elements_in_file(elements_in_file) {
      header_pos = ofs.tellp();
      write_u64(ofs, elements_in_file);
    }

    explicit Writer(std::ofstream &ofs) : Writer(ofs, 0) {}

    template <typename T> void write_value(const T &input) { ofs << input; }
    template <typename T> void fix_headers(const T &input) {
      elements_in_file = input;
      auto offset = ofs.tellp();
      ofs.seekp(header_pos, std::ios::beg);
      write_u64(ofs, input);
      ofs.seekp(offset);
    }
  };
};
} // namespace k2cache
#endif // ULHEADERIOHANDLER_CUSTOM_SERIALIZATION_HPP
