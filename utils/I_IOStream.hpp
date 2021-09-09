//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_I_IOSTREAM_HPP
#define RDFCACHEK2_I_IOSTREAM_HPP

#include <ios>
#include <iostream>

class I_IOStream {
public:
  virtual ~I_IOStream() = default;
  virtual std::iostream &get_stream() = 0;

  virtual void seekg(std::streamoff offset, std::ios_base::seekdir way) = 0;
  virtual std::streampos tellg() = 0;

  virtual void seekp(std::streamoff offset, std::ios_base::seekdir way) = 0;
  virtual std::streampos tellp() = 0;

  virtual void flush() = 0;
};

#endif // RDFCACHEK2_I_IOSTREAM_HPP
