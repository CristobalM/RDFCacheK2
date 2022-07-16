//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_FILEIOSTREAM_HPP
#define RDFCACHEK2_FILEIOSTREAM_HPP

#include <fstream>
#include <ios>
#include <string>

#include "I_IOStream.hpp"
namespace k2cache {
class FileIOStream : public I_IOStream {
  std::fstream fs;

public:
  FileIOStream(const std::string &filename, std::ios::openmode openmode);
  void seekg(std::streamoff offset, std::ios_base::seekdir way) override;
  std::istream &get_istream() override;
  std::ostream &get_ostream() override;
  std::iostream &get_stream() override;
  std::streampos tellg() override;
  void seekp(std::streamoff offset, std::ios_base::seekdir way) override;
  std::streampos tellp() override;
  void flush() override;
  explicit operator bool() const override;
};
} // namespace k2cache
#endif // RDFCACHEK2_FILEIOSTREAM_HPP
