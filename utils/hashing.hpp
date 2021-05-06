#ifndef RDFCACHEK2__HASHING_HPP_
#define RDFCACHEK2__HASHING_HPP_

#include <array>
#include <string>
#include <vector>

std::array<char, 16> md5calc(const std::string &input);
std::array<char, 16> md5calc(const std::vector<char> &input);
std::string md5_human_readable(const std::array<char, 16> &digest);
std::string md5_human_readable_lowercase(const std::array<char, 16> &digest);
std::string md5_human_readable_lowercase(const std::string &input);

std::array<char, 20> sha1calc(const std::string &input);
std::array<char, 20> sha1calc(const std::vector<char> &input);
std::string sha1_human_readable(const std::array<char, 20> &digest);
std::string sha1_human_readable_lowercase(const std::array<char, 20> &digest);
std::string sha1_human_readable_lowercase(const std::string &input);

std::array<char, 28> sha224calc(const std::string &input);
std::array<char, 28> sha224calc(const std::vector<char> &input);
std::string sha224_human_readable(const std::array<char, 28> &digest);
std::string sha224_human_readable_lowercase(const std::array<char, 28> &digest);
std::string sha224_human_readable_lowercase(const std::string &input);

std::array<char, 32> sha256calc(const std::string &input);
std::array<char, 32> sha256calc(const std::vector<char> &input);
std::string sha256_human_readable(const std::array<char, 32> &digest);
std::string sha256_human_readable_lowercase(const std::array<char, 32> &digest);
std::string sha256_human_readable_lowercase(const std::string &input);

std::array<char, 48> sha384calc(const std::string &input);
std::array<char, 48> sha384calc(const std::vector<char> &input);
std::string sha384_human_readable(const std::array<char, 48> &digest);
std::string sha384_human_readable_lowercase(const std::array<char, 48> &digest);
std::string sha384_human_readable_lowercase(const std::string &input);

std::array<char, 64> sha512calc(const std::string &input);
std::array<char, 64> sha512calc(const std::vector<char> &input);
std::string sha512_human_readable(const std::array<char, 64> &digest);
std::string sha512_human_readable_lowercase(const std::array<char, 64> &digest);
std::string sha512_human_readable_lowercase(const std::string &input);

#endif /* RDFCACHEK2__HASHING_HPP_ */
