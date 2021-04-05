#ifndef RDFCACHEK2__HASHING_HPP_
#define RDFCACHEK2__HASHING_HPP_

#include <array>
#include <string>
#include <vector>

std::array<char, 16> md5calc(const std::string &input);
std::array<char, 16> md5calc(const std::vector<char> &input);
std::string md5_human_readable(const std::array<char, 16> &digest);

#endif /* RDFCACHEK2__HASHING_HPP_ */
