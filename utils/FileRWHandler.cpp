//
// Created by cristobal on 9/6/21.
//

#include <filesystem>
#include <fstream>

#include "FileIOStream.hpp"
#include "FileIStream.hpp"
#include "FileOStream.hpp"
#include "FileRWHandler.hpp"

namespace fs = std::filesystem;

std::unique_ptr<I_OStream>
FileRWHandler::get_writer(std::ios::openmode openmode) {
  return std::make_unique<FileOStream>(filename, std::ios::out | openmode);
}
FileRWHandler::FileRWHandler(std::string filename)
    : filename(std::move(filename)) {}
std::unique_ptr<I_IStream>
FileRWHandler::get_reader(std::ios::openmode openmode) {
  return std::make_unique<FileIStream>(filename, std::ios::in | openmode);
}
bool FileRWHandler::exists() { return fs::exists(filename); }
std::unique_ptr<I_OStream>
FileRWHandler::get_writer_temp(std::ios::openmode openmode) {
  return std::make_unique<FileOStream>(filename + ".tmp",
                                       std::ios::out | openmode);
}

void FileRWHandler::commit_temp_writer() {
  std::rename(filename.c_str(), (filename + ".deleting").c_str());
  std::rename((filename + ".tmp").c_str(), filename.c_str());
  std::remove((filename + ".deleting").c_str());
}
std::unique_ptr<I_IOStream>
FileRWHandler::get_reader_writer(std::ios::openmode openmode) {
  return std::make_unique<FileIOStream>(filename, openmode);
}
void FileRWHandler::clean() { std::remove(filename.c_str()); }
