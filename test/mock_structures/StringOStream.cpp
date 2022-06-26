//
// Created by cristobal on 9/7/21.
//
#include "StringOStream.hpp"

namespace k2cache {
StringOStream::StringOStream(std::string &data, std::ios::openmode openmode)
    : data(data), oss(data, openmode) {}

StringOStream::~StringOStream() { internal_flush(); }

void StringOStream::flush() { internal_flush(); }

std::ostream &StringOStream::get_ostream() { return oss; }

void StringOStream::internal_flush() { data = oss.str(); }
void StringOStream::seekp(std::streamoff offset, std::ios_base::seekdir way) {
  oss.seekp(offset, way);
}
std::streampos StringOStream::tellp() { return oss.tellp(); }
} // namespace k2cache