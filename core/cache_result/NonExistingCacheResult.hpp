//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_NONEXISTINGCACHERESULT_HPP
#define RDFCACHEK2_NONEXISTINGCACHERESULT_HPP


#include <string>
#include "CacheResult.hpp"

class NonExistingCacheResult : CacheResult {
  bool exists() override;
  std::string as_string() override;
};


#endif //RDFCACHEK2_NONEXISTINGCACHERESULT_HPP
