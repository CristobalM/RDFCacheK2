//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_STRINGOSTREAM_HPP
#define RDFCACHEK2_STRINGOSTREAM_HPP

#include <ios>
#include <ostream>
#include <sstream>
#include <string>

#include <I_OStream.hpp>
namespace k2cache {

struct StringOStream : public I_OStream {
  std::string &data;
  std::ostringstream oss;

public:
  StringOStream(std::string &data, std::ios::openmode openmode);
  ~StringOStream() override;
  void flush() override;
  std::ostream &get_ostream() override;
  void seekp(std::streamoff offset, std::ios_base::seekdir way) override;
  std::streampos tellp() override;

private:
  void internal_flush();
};
} // namespace k2cache
#endif // RDFCACHEK2_STRINGOSTREAM_HPP
