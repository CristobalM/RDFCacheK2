//
// Created by cristobal on 21-11-21.
//

#ifndef ULConnectorWHeaderCustomSerialization_CUSTOM_SERIALIZATION_HPP
#define ULConnectorWHeaderCustomSerialization_CUSTOM_SERIALIZATION_HPP

#include "serialization_util.hpp"
#include <fstream>
namespace k2cache {
class ULConnectorWHeaderCustomSerialization {
  unsigned long value;

public:
  static constexpr bool fixed_size = true;

  explicit ULConnectorWHeaderCustomSerialization(unsigned long value)
      : value(value) {}

  ULConnectorWHeaderCustomSerialization() : value(0) {}

  ULConnectorWHeaderCustomSerialization(
      ULConnectorWHeaderCustomSerialization &&other) noexcept
      : value(other.value) {}

  ULConnectorWHeaderCustomSerialization(
      const ULConnectorWHeaderCustomSerialization &other) = default;

  ULConnectorWHeaderCustomSerialization &
  operator=(ULConnectorWHeaderCustomSerialization &&other) noexcept {
    this->value = other.value;
    return *this;
  }

  ULConnectorWHeaderCustomSerialization &
  operator=(const ULConnectorWHeaderCustomSerialization &other) = default;

  struct Comparator {
    bool operator()(const ULConnectorWHeaderCustomSerialization &lhs,
                    const ULConnectorWHeaderCustomSerialization &rhs) {
      return lhs.value < rhs.value;
    }
  };

  friend std::ostream &
  operator<<(std::ostream &os,
             const ULConnectorWHeaderCustomSerialization &data);

  bool operator==(const ULConnectorWHeaderCustomSerialization &other) const {
    return value == other.value;
  }

  bool operator!=(const ULConnectorWHeaderCustomSerialization &other) const {
    return value != other.value;
  }

  static bool read_value(std::ifstream &ifs,
                         ULConnectorWHeaderCustomSerialization &next_val) {
    unsigned long value = read_u64(ifs);
    next_val = ULConnectorWHeaderCustomSerialization(value);
    return true;
  }

  static size_t size() { return sizeof(unsigned long); }
};

std::ostream &operator<<(std::ostream &os,
                         const ULConnectorWHeaderCustomSerialization &data) {
  write_u64(os, data.value);
  return os;
}
} // namespace k2cache
#endif // ULConnectorWHeaderCustomSerialization_CUSTOM_SERIALIZATION_HPP
