//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_FILEOSTREAM_HPP
#define RDFCACHEK2_FILEOSTREAM_HPP

#include "I_OStream.hpp"
#include <fstream>
#include <ios>
#include <string>
namespace k2cache {

class FileOStream : public I_OStream {
  std::ofstream ofs;

public:
  FileOStream(const std::string &filename, std::ios::openmode openmode);
  void flush() override;
  std::ostream &get_ostream() override;
  void seekp(std::streamoff offset, std::ios_base::seekdir way) override;
  std::streampos tellp() override;
};
} // namespace k2cache
#endif // RDFCACHEK2_FILEOSTREAM_HPP
