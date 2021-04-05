#include "KeyPairStr.hpp"

KeyPairStr::KeyPairStr() {}

KeyPairStr::KeyPairStr(std::string &&first, std::string &&second)
    : first_var(std::move(first)), second_var(std::move(second)) {}

KeyPairStr::KeyPairStr(const std::string &first, const std::string &second)
    : first_var(first), second_var(second) {}

KeyPairStr::KeyPairStr(KeyPairStr &&other) noexcept
    : first_var(std::move(other.first_var)),
      second_var(std::move(other.second_var)) {}

KeyPairStr::KeyPairStr(const KeyPairStr &other)
    : first_var(other.first_var), second_var(other.second_var) {}

KeyPairStr &KeyPairStr::operator=(KeyPairStr &&other) noexcept {
  this->first_var = std::move(other.first_var);
  this->second_var = std::move(other.second_var);
  return *this;
}

KeyPairStr &KeyPairStr::operator=(const KeyPairStr &other) {
  this->first_var = other.first_var;
  this->second_var = other.second_var;
  return *this;
}

bool operator==(const KeyPairStr &lhs, const KeyPairStr &rhs) {
  return lhs.first_var == rhs.first_var && lhs.second_var == rhs.second_var;
}
