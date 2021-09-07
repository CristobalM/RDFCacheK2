//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_FILEOSTREAM_HPP
#define RDFCACHEK2_FILEOSTREAM_HPP

#include "I_OStream.hpp"
#include <ios>
#include <string>
#include <fstream>

class FileOStream : public I_OStream {
  std::ofstream ofs;
public:
  FileOStream(const std::string &filename, std::ios::openmode openmode);
  void flush() override;
  std::ostream &get_stream() override;
};

#endif // RDFCACHEK2_FILEOSTREAM_HPP
