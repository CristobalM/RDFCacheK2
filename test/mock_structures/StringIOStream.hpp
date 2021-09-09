//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_STRINGIOSTREAM_HPP
#define RDFCACHEK2_STRINGIOSTREAM_HPP

#include <ios>
#include <sstream>

#include <I_IOStream.hpp>
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
};

#endif // RDFCACHEK2_STRINGIOSTREAM_HPP
