//
// Created by cristobal on 9/6/21.
//

#include <filesystem>
#include <fstream>

#include "FileRWHandler.hpp"

namespace fs = std::filesystem;

std::unique_ptr<std::ostream>
FileRWHandler::get_writer(std::ios::openmode openmode) {
  return std::make_unique<std::ofstream>(filename, std::ios::out | openmode);
}
FileRWHandler::FileRWHandler(std::string filename)
    : filename(std::move(filename)) {}
std::unique_ptr<std::istream>
FileRWHandler::get_reader(std::ios::openmode openmode) {
  return std::make_unique<std::ifstream>(filename, std::ios::in | openmode);
}
bool FileRWHandler::exists() { return fs::exists(filename); }
std::unique_ptr<std::ostream>
FileRWHandler::get_writer_temp(std::ios::openmode openmode) {
  return std::make_unique<std::ofstream>(filename + ".tmp",
                                         std::ios::out | openmode);
}

void FileRWHandler::commit_temp_writer() {
  std::rename(filename.c_str(), (filename + ".deleting").c_str());
  std::rename((filename + ".tmp").c_str(), filename.c_str());
  std::remove((filename + ".deleting").c_str());
}
