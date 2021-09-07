//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_I_ISTREAM_HPP
#define RDFCACHEK2_I_ISTREAM_HPP
#include <ios>
class I_IStream{
public:
  virtual ~I_IStream() = default;
  virtual void seekg(std::streamoff offset, std::ios_base::seekdir way) = 0;
  virtual explicit operator bool() const = 0;
  virtual std::istream &get_stream() = 0;
};
#endif // RDFCACHEK2_I_ISTREAM_HPP
