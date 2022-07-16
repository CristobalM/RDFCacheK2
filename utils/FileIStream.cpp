//
// Created by cristobal on 07-09-21.
//

#include "FileIStream.hpp"
namespace k2cache {
void FileIStream::seekg(std::streamoff offset, std::ios_base::seekdir way) {
  ifs.seekg(offset, way);
}
FileIStream::operator bool() const { return ifs.operator bool(); }
std::istream &FileIStream::get_istream() { return ifs; }
FileIStream::FileIStream(const std::string &filename,
                         std::ios::openmode openmode)
    : ifs(filename, openmode) {}
std::streampos FileIStream::tellg() { return ifs.tellg(); }
} // namespace k2cache
