//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHERESULT_HPP
#define RDFCACHEK2_CACHERESULT_HPP

#include <string>

class CacheResult {
public:
  virtual bool exists() = 0;
  virtual std::string as_string() = 0;
};

#endif // RDFCACHEK2_CACHERESULT_HPP
