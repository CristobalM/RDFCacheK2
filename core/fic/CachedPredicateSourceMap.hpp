//
// Created by cristobal on 03-10-22.
//

#ifndef RDFCACHEK2_CACHEDPREDICATESOURCEMAP_HPP
#define RDFCACHEK2_CACHEDPREDICATESOURCEMAP_HPP

#include "I_CachedPredicateSource.hpp"
namespace k2cache{
class CachedPredicateSourceMap{
public:
  virtual ~CachedPredicateSourceMap() = default;
  std::iterator<std::pair<unsigned long, std::unique_ptr<I_CachedPredicateSource>&>> find(unsigned long predicate_id);
};
}

#endif // RDFCACHEK2_CACHEDPREDICATESOURCEMAP_HPP
