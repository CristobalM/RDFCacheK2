//
// Created by cristobal on 9/7/21.
//

#include "StringIStream.hpp"
namespace k2cache {

StringIStream::StringIStream(std::string &data, std::ios::openmode openmode)
    : data(data), iss(data, openmode) {}

void StringIStream::seekg(std::streamoff offset, std::ios_base::seekdir way) {
  iss.seekg(offset, way);
}

std::istream &StringIStream::get_istream() { return iss; }

StringIStream::operator bool() const {
  return iss.operator bool() && !iss.eof() && iss.good();
}
std::streampos StringIStream::tellg() { return iss.tellg(); }
} // namespace k2cache