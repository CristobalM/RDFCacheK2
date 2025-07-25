//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_I_ISTREAM_HPP
#define RDFCACHEK2_I_ISTREAM_HPP
#include <ios>
namespace k2cache {
struct I_IStream {
  virtual ~I_IStream() = default;
  virtual void seekg(std::streamoff offset, std::ios_base::seekdir way) = 0;
  void seekg(std::streamoff offset) { seekg(offset, std::ios_base::beg); };
  virtual std::streampos tellg() = 0;

  virtual explicit operator bool() const = 0;
  virtual std::istream &get_istream() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_I_ISTREAM_HPP
