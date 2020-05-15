//
// Created by Cristobal Miranda, 2020
//

#include "NonExistingCacheResult.hpp"

bool NonExistingCacheResult::exists() {
  return false;
}

std::string NonExistingCacheResult::as_string() {
  return nullptr;
}
