//
// Created by cristobal on 7/22/21.
//

#ifndef RDFCACHEK2_EMPTYSCANNER_HPP
#define RDFCACHEK2_EMPTYSCANNER_HPP

#include "k2tree/K2QStateWrapper.hpp"
#include "k2tree/K2TreeScanner.hpp"
namespace k2cache {
class EmptyScanner : public K2TreeScanner {
public:
  explicit EmptyScanner(K2TreeMixed &k2tree);
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
  void reset_scan() override;
  unsigned long get_band_value() override;
  K2TreeMixed &get_tree() override;
  K2QStateWrapper &get_k2qw() override;

private:
  K2QStateWrapper stw;
  K2TreeMixed &k2tree;
};
} // namespace k2cache
#endif // RDFCACHEK2_EMPTYSCANNER_HPP
