//
// Created by cristobal on 07-09-21.
//

#include "FileOStream.hpp"
void FileOStream::flush() {
  ofs.flush();
}
FileOStream::FileOStream(const std::string &filename,
                         std::ios::openmode openmode)
: ofs(filename, openmode)
{}
std::ostream &FileOStream::get_stream() { return ofs; }
