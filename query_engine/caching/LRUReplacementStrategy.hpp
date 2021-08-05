//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_LRUREPLACEMENTSTRATEGY_HPP
#define RDFCACHEK2_LRUREPLACEMENTSTRATEGY_HPP

#include <map>
#include <unordered_map>
class LRUReplacementStrategy {
  long low;
  long high;
  std::map<unsigned long, long> key_mapping_to_lru_value;

public:
  LRUReplacementStrategy();

  LRUReplacementStrategy(LRUReplacementStrategy &other) = delete;
  LRUReplacementStrategy &operator=(LRUReplacementStrategy &other) = delete;

  bool operator()(unsigned long lhs, unsigned long rhs) const;

  void hit_key(unsigned long key);
  void remove_key(unsigned long key);
};

#endif // RDFCACHEK2_LRUREPLACEMENTSTRATEGY_HPP
