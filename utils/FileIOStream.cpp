//
// Created by cristobal on 9/7/21.
//

#include "FileIOStream.hpp"
namespace k2cache {
void FileIOStream::seekg(std::streamoff offset, std::ios_base::seekdir way) {
  fs.seekg(offset, way);
}
std::iostream &FileIOStream::get_stream() { return fs; }
FileIOStream::FileIOStream(const std::string &filename,
                           std::ios::openmode openmode)
    : fs(filename, std::ios::in | std::ios::out | openmode) {}
std::streampos FileIOStream::tellg() { return fs.tellg(); }
void FileIOStream::seekp(std::streamoff offset, std::ios_base::seekdir way) {
  fs.seekp(offset, way);
}
std::streampos FileIOStream::tellp() { return fs.tellp(); }
void FileIOStream::flush() { fs.flush(); }
std::istream &FileIOStream::get_istream() { return fs; }
std::ostream &FileIOStream::get_ostream() { return fs; }
FileIOStream::operator bool() const { return fs.operator bool(); }
} // namespace k2cache
