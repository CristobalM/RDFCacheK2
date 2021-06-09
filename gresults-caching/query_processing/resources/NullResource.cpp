//
// Created by cristobal on 4/21/21.
//

#include "NullResource.hpp"
bool NullResource::operator==(const TermResource &rhs) const {
  return rhs.is_null();
}

bool NullResource::is_null() const { return true; }
