//
// Created by cristobal on 9/7/21.
//

#include "FHMock.hpp"
#include "StringIOStream.hpp"
#include "StringIStream.hpp"
#include "StringOStream.hpp"

namespace k2cache {
FHMock::FHMock(std::string &data) : data(data) {}

std::unique_ptr<I_OStream> FHMock::get_writer(std::ios::openmode) {
  return std::make_unique<StringOStream>(data, std::ios::out | std::ios::app |
                                                   std::ios::binary);
}
std::unique_ptr<I_IStream> FHMock::get_reader(std::ios::openmode) {
  return std::make_unique<StringIStream>(data, std::ios::in | std::ios::binary);
}
bool FHMock::exists() { return true; }
std::unique_ptr<I_OStream> FHMock::get_writer_temp(std::ios::openmode) {
  return std::make_unique<StringOStream>(
      temp_data, std::ios::out | std::ios::trunc | std::ios::binary);
}
void FHMock::commit_temp_writer() { data = temp_data; }
std::unique_ptr<I_IOStream>
FHMock::get_reader_writer(std::ios::openmode openmode) {
  return std::make_unique<StringIOStream>(
      temp_data, std::ios::in | std::ios::out | openmode);
}
void FHMock::clean() {
  data = "";
  temp_data = "";
}
} // namespace k2cache
