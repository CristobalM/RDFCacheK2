//
// Created by Cristobal Miranda, 2020
//

#include "raptor_util.hpp"
#include <stdlib.h>

std::string get_term_value(raptor_term *term) {
  char *value;
  size_t value_sz;
  value =
      reinterpret_cast<char *>(raptor_term_to_counted_string(term, &value_sz));
  auto result = std::string(value, value_sz);
  free(value);

  return result;
}
