//
// Created by cristobal on 07-09-21.
//

#include "FileOStream.hpp"
void FileOStream::flush() { ofs.flush(); }
FileOStream::FileOStream(const std::string &filename,
                         std::ios::openmode openmode)
    : ofs(filename, openmode) {}
std::ostream &FileOStream::get_ostream() { return ofs; }
void FileOStream::seekp(std::streamoff offset, std::ios_base::seekdir way) {
  ofs.seekp(offset, way);
}
std::streampos FileOStream::tellp() { return ofs.tellp(); }
