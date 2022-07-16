//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_STRINGIOSTREAM_HPP
#define RDFCACHEK2_STRINGIOSTREAM_HPP

#include <ios>
#include <sstream>

#include <I_IOStream.hpp>
namespace k2cache {
class StringIOStream : public I_IOStream {
  std::string &data;
  std::stringstream ss;

public:
  void seekg(std::streamoff offset, std::ios_base::seekdir way) override;
  std::iostream &get_stream() override;
  std::streampos tellg() override;
  void seekp(std::streamoff offset, std::ios_base::seekdir way) override;
  std::streampos tellp() override;
  StringIOStream(std::string &data, std::ios::openmode openmode);
  void flush() override;
  explicit operator bool() const override;
  std::istream &get_istream() override;
  std::ostream &get_ostream() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_STRINGIOSTREAM_HPP
