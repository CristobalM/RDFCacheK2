#include "hash_pair.hpp"
#include <functional>

size_t hash_pair::operator()(const KeyPairStr &p) const {
  auto hash1 = std::hash<std::string>{}(p.first_var);
  auto hash2 = std::hash<std::string>{}(p.second_var);
  return hash1 ^ hash2;
}
