//
// Created by cristobal on 7/22/21.
//

#ifndef RDFCACHEK2_FULLSCANNER_HPP
#define RDFCACHEK2_FULLSCANNER_HPP

#include "K2QStateWrapper.hpp"
#include "K2TreeScanner.hpp"
#include <k2node.h>
class FullScanner : public K2TreeScanner {
public:
  explicit FullScanner(K2TreeMixed &k2tree);

  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
  void reset_scan() override;
  unsigned long get_band_value() override;
  K2TreeMixed &get_tree() override;
  ~FullScanner() override;
  K2QStateWrapper &get_k2qw() override;
  unsigned long get_size() override;
  bool has_point(unsigned long col, unsigned long row) override;

private:
  K2QStateWrapper stw;
  k2node_lazy_handler_naive_scan_t lazy_handler{};
  K2TreeMixed &k2tree;
};

#endif // RDFCACHEK2_FULLSCANNER_HPP
