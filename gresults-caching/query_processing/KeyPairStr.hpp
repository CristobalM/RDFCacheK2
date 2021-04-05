//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_KEYPAIRSTR_HPP
#define RDFCACHEK2_KEYPAIRSTR_HPP

#include <memory>
#include <string>

struct KeyPairStr {
  std::string first_var;
  std::string second_var;

  KeyPairStr();
  KeyPairStr(std::string &&first, std::string &&second);
  KeyPairStr(const std::string &first, const std::string &second);
  KeyPairStr(KeyPairStr &&other) noexcept;
  KeyPairStr(const KeyPairStr &other);
  KeyPairStr &operator=(KeyPairStr &&other) noexcept;
  KeyPairStr &operator=(const KeyPairStr &other);
  friend bool operator==(const KeyPairStr &lhs, const KeyPairStr &rhs);
};

#endif /* RDFCACHEK2_KEYPAIRSTR_HPP */
