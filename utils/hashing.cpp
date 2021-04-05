

#include "hashing.hpp"

#include <algorithm>
#include <openssl/md5.h>

std::array<char, 16> md5calc(const std::string &input) {
  std::array<char, 16> result;

  MD5(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
      reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::array<char, 16> md5calc(const std::vector<char> &input) {
  std::array<char, 16> result;

  MD5(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
      reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::string md5_human_readable(const std::array<char, 16> &digest) {
  static const char hexchars[] = "0123456789abcdef";

  std::string result;

  for (int i = 0; i < 16; i++) {
    unsigned char b = digest[i];
    char hex[3];

    hex[0] = hexchars[b >> 4];
    hex[1] = hexchars[b & 0xF];
    hex[2] = 0;

    result.append(hex);
  }

  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}
