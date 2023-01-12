//
// Created by Cristobal Miranda on 09-01-23.
//

#ifndef RDFCACHEK2_K2TREESFEED_HPP
#define RDFCACHEK2_K2TREESFEED_HPP
#include <memory>
#include "k2tree/K2TreeMixed.hpp"
namespace k2cache{

struct K2TreeContainer{
  uint64_t predicate{};
  std::unique_ptr<K2TreeMixed> tree;
};

struct K2TreesFeed {
  virtual ~K2TreesFeed() = default;
  virtual bool has_next() = 0;
  virtual K2TreeContainer get_next() = 0;
};
}
#endif // RDFCACHEK2_K2TREESFEED_HPP
