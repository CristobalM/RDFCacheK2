//
// Created by cristobal on 9/1/21.
//

#ifndef RDFCACHEK2_NULLSCANNER_HPP
#define RDFCACHEK2_NULLSCANNER_HPP

#include "K2QStateWrapper.hpp"
#include "K2TreeMixed.hpp"
#include "K2TreeScanner.hpp"
namespace k2cache {
class NullScanner : public K2TreeScanner {
public:
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
  void reset_scan() override;
  unsigned long get_band_value() override;
  K2TreeMixed &get_tree() override;
  K2QStateWrapper &get_k2qw() override;
  ~NullScanner() override;
};
} // namespace k2cache
#endif // RDFCACHEK2_NULLSCANNER_HPP
