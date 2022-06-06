//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_I_IOSTREAM_HPP
#define RDFCACHEK2_I_IOSTREAM_HPP

#include "I_IStream.hpp"
#include "I_OStream.hpp"
#include <ios>
#include <iostream>

class I_IOStream : public I_OStream, public I_IStream {
public:
  virtual std::iostream &get_stream() = 0;
};

#endif // RDFCACHEK2_I_IOSTREAM_HPP
