

#include "hashing.hpp"

#include <algorithm>
#include <openssl/md5.h>
#include <openssl/sha.h>
namespace k2cache {
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

static std::string
md5_human_readable_common(const std::array<char, 16> &digest) {
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

  return result;
}

std::string md5_human_readable(const std::array<char, 16> &digest) {
  auto result = md5_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}

std::string md5_human_readable_lowercase(const std::array<char, 16> &digest) {
  auto result = md5_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::tolower(c); });
  return result;
}

std::string md5_human_readable_lowercase(const std::string &input) {
  auto digest = md5calc(input);
  return md5_human_readable_lowercase(digest);
}

std::array<char, 20> sha1calc(const std::string &input) {
  std::array<char, 20> result;

  SHA1(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
       reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::array<char, 20> sha1calc(const std::vector<char> &input) {
  std::array<char, 20> result;

  SHA1(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
       reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

static std::string
sha1_human_readable_common(const std::array<char, 20> &digest) {
  static const char hexchars[] = "0123456789abcdef";

  std::string result;

  for (int i = 0; i < 20; i++) {
    unsigned char b = digest[i];
    char hex[3];

    hex[0] = hexchars[b >> 4];
    hex[1] = hexchars[b & 0xF];
    hex[2] = 0;

    result.append(hex);
  }

  return result;
}

std::string sha1_human_readable(const std::array<char, 20> &digest) {
  auto result = sha1_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}

std::string sha1_human_readable_lowercase(const std::array<char, 20> &digest) {
  auto result = sha1_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::tolower(c); });
  return result;
}

std::string sha1_human_readable_lowercase(const std::string &input) {
  auto digest = sha1calc(input);
  return sha1_human_readable_lowercase(digest);
}

std::array<char, 28> sha224calc(const std::string &input) {
  std::array<char, 28> result;

  SHA224(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::array<char, 28> sha224calc(const std::vector<char> &input) {
  std::array<char, 28> result;

  SHA224(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

static std::string
sha224_human_readable_common(const std::array<char, 28> &digest) {
  static const char hexchars[] = "0123456789abcdef";

  std::string result;

  for (int i = 0; i < 28; i++) {
    unsigned char b = digest[i];
    char hex[3];

    hex[0] = hexchars[b >> 4];
    hex[1] = hexchars[b & 0xF];
    hex[2] = 0;

    result.append(hex);
  }

  return result;
}

std::string sha224_human_readable(const std::array<char, 28> &digest) {
  auto result = sha224_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}

std::string
sha224_human_readable_lowercase(const std::array<char, 28> &digest) {
  auto result = sha224_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::tolower(c); });
  return result;
}

std::string sha224_human_readable_lowercase(const std::string &input) {
  auto digest = sha224calc(input);
  return sha224_human_readable_lowercase(digest);
}

std::array<char, 32> sha256calc(const std::string &input) {
  std::array<char, 32> result;

  SHA256(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::array<char, 32> sha256calc(const std::vector<char> &input) {
  std::array<char, 32> result;

  SHA256(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

static std::string
sha256_human_readable_common(const std::array<char, 32> &digest) {
  static const char hexchars[] = "0123456789abcdef";

  std::string result;

  for (int i = 0; i < 32; i++) {
    unsigned char b = digest[i];
    char hex[3];

    hex[0] = hexchars[b >> 4];
    hex[1] = hexchars[b & 0xF];
    hex[2] = 0;

    result.append(hex);
  }

  return result;
}

std::string sha256_human_readable(const std::array<char, 32> &digest) {
  auto result = sha256_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}

std::string
sha256_human_readable_lowercase(const std::array<char, 32> &digest) {
  auto result = sha256_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::tolower(c); });
  return result;
}

std::string sha256_human_readable_lowercase(const std::string &input) {
  auto digest = sha256calc(input);
  return sha256_human_readable_lowercase(digest);
}

std::array<char, 48> sha384calc(const std::string &input) {
  std::array<char, 48> result;

  SHA384(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::array<char, 48> sha384calc(const std::vector<char> &input) {
  std::array<char, 48> result;

  SHA384(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

static std::string
sha384_human_readable_common(const std::array<char, 48> &digest) {
  static const char hexchars[] = "0123456789abcdef";

  std::string result;

  for (int i = 0; i < 48; i++) {
    unsigned char b = digest[i];
    char hex[3];

    hex[0] = hexchars[b >> 4];
    hex[1] = hexchars[b & 0xF];
    hex[2] = 0;

    result.append(hex);
  }

  return result;
}

std::string sha384_human_readable(const std::array<char, 48> &digest) {
  auto result = sha384_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}

std::string
sha384_human_readable_lowercase(const std::array<char, 48> &digest) {
  auto result = sha384_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::tolower(c); });
  return result;
}

std::string sha384_human_readable_lowercase(const std::string &input) {
  auto digest = sha384calc(input);
  return sha384_human_readable_lowercase(digest);
}

std::array<char, 64> sha512calc(const std::string &input) {
  std::array<char, 64> result;

  SHA512(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

std::array<char, 64> sha512calc(const std::vector<char> &input) {
  std::array<char, 64> result;

  SHA512(reinterpret_cast<const unsigned char *>(input.data()), input.size(),
         reinterpret_cast<unsigned char *>(result.data()));
  return result;
}

static std::string
sha512_human_readable_common(const std::array<char, 64> &digest) {
  static const char hexchars[] = "0123456789abcdef";

  std::string result;

  for (int i = 0; i < 64; i++) {
    unsigned char b = digest[i];
    char hex[3];

    hex[0] = hexchars[b >> 4];
    hex[1] = hexchars[b & 0xF];
    hex[2] = 0;

    result.append(hex);
  }

  return result;
}

std::string sha512_human_readable(const std::array<char, 64> &digest) {
  auto result = sha512_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::toupper(c); });
  return result;
}

std::string
sha512_human_readable_lowercase(const std::array<char, 64> &digest) {
  auto result = sha512_human_readable_common(digest);
  std::for_each(result.begin(), result.end(),
                [](char &c) { c = ::tolower(c); });
  return result;
}

std::string sha512_human_readable_lowercase(const std::string &input) {
  auto digest = sha512calc(input);
  return sha512_human_readable_lowercase(digest);
}
} // namespace k2cache
