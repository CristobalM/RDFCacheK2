//
// Created by cristobal on 9/7/21.
//

#include "StringIOStream.hpp"
namespace k2cache {
void StringIOStream::seekg(std::streamoff offset, std::ios_base::seekdir way) {
  ss.seekg(offset, way);
}
std::iostream &StringIOStream::get_stream() { return ss; }
std::streampos StringIOStream::tellg() { return ss.tellg(); }
void StringIOStream::seekp(std::streamoff offset, std::ios_base::seekdir way) {
  ss.seekp(offset, way);
}
std::streampos StringIOStream::tellp() { return ss.tellp(); }
StringIOStream::StringIOStream(std::shared_ptr<std::string> data, std::ios::openmode openmode)
    : data(std::move(data)),
      ss(*this->data, openmode | std::ios::in | std::ios::out) {}

void StringIOStream::flush() { *data = ss.str(); }
StringIOStream::operator bool() const { return ss.operator bool(); }
std::istream &StringIOStream::get_istream() { return ss; }
std::ostream &StringIOStream::get_ostream() { return ss; }
} // namespace k2cache