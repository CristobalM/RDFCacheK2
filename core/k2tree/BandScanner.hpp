//
// Created by cristobal on 7/22/21.
//

#ifndef RDFCACHEK2_BANDSCANNER_HPP
#define RDFCACHEK2_BANDSCANNER_HPP

#include "K2QStateWrapper.hpp"
#include "K2TreeScanner.hpp"
#include "k2node.h"
namespace k2cache {
class BandScanner : public K2TreeScanner {
public:
  BandScanner(K2TreeMixed &k2tree, uint64_t band,
              K2TreeScanner::BandType band_type);
  bool has_next() override;
  std::pair<uint64_t, uint64_t> next() override;
  ~BandScanner() override;
  void reset_scan() override;
  K2TreeMixed &get_tree() override;
  uint64_t get_band_value() override;
  K2QStateWrapper &get_k2qw() override;

private:
  // struct k2qstate st {};
  K2QStateWrapper stw;
  uint64_t band;
  K2TreeScanner::BandType band_type;
  k2node_lazy_handler_report_band_t lazy_handler{};
  K2TreeMixed &k2tree;
};
} // namespace k2cache
#endif // RDFCACHEK2_BANDSCANNER_HPP
