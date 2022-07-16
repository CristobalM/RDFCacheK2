//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_I_OSTREAM_HPP
#define RDFCACHEK2_I_OSTREAM_HPP
#include <ostream>
namespace k2cache {
class I_OStream {
public:
  virtual ~I_OStream() = default;
  virtual void flush() = 0;
  virtual std::ostream &get_ostream() = 0;
  virtual void seekp(std::streamoff offset, std::ios_base::seekdir way) = 0;
  virtual std::streampos tellp() = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_I_OSTREAM_HPP
