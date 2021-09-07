//
// Created by cristobal on 07-09-21.
//

#ifndef RDFCACHEK2_FILEISTREAM_HPP
#define RDFCACHEK2_FILEISTREAM_HPP

#include "I_IStream.hpp"
#include <ios>
#include <fstream>

class FileIStream : public I_IStream {
  std::ifstream ifs;
public:
  void seekg(std::streamoff offset, std::ios_base::seekdir way) override;
  explicit operator bool() const override;
  std::istream &get_stream() override;

  FileIStream(const std::string &filename, std::ios::openmode openmode);

public:
};

#endif // RDFCACHEK2_FILEISTREAM_HPP
