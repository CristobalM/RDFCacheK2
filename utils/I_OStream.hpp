//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_I_OSTREAM_HPP
#define RDFCACHEK2_I_OSTREAM_HPP
#include <ostream>
class I_OStream{
public:
  virtual ~I_OStream() = default;
  virtual void flush() = 0;
  virtual std::ostream &get_stream() = 0;
};

#endif // RDFCACHEK2_I_OSTREAM_HPP
