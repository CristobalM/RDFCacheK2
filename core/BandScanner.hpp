//
// Created by cristobal on 7/22/21.
//

#ifndef RDFCACHEK2_BANDSCANNER_HPP
#define RDFCACHEK2_BANDSCANNER_HPP

#include "K2TreeScanner.hpp"
#include <TimeControl.hpp>
#include <k2node.h>

class BandScanner : public K2TreeScanner {
public:
  BandScanner(K2TreeMixed &k2tree, unsigned long band,
              K2TreeScanner::BandType band_type, TimeControl &time_control);
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
  ~BandScanner() override;
  void reset_scan() override;
  K2TreeMixed &get_tree() override;
  unsigned long get_band_value() override;

private:
  struct k2qstate st {};
  unsigned long band;
  K2TreeScanner::BandType band_type;
  k2node_lazy_handler_report_band_t lazy_handler{};
  K2TreeMixed &k2tree;
  TimeControl &time_control;
};

#endif // RDFCACHEK2_BANDSCANNER_HPP
