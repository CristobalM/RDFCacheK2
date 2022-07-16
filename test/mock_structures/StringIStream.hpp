//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_STRINGISTREAM_HPP
#define RDFCACHEK2_STRINGISTREAM_HPP

#include <ios>
#include <istream>
#include <sstream>
#include <string>

#include <I_IStream.hpp>
namespace k2cache {
struct StringIStream : public I_IStream {
  std::string data;
  std::istringstream iss;

public:
  StringIStream(std::string &data, std::ios::openmode openmode);
  void seekg(std::streamoff offset, std::ios_base::seekdir way) override;
  explicit operator bool() const override;
  std::istream &get_istream() override;
  std::streampos tellg() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_STRINGISTREAM_HPP
