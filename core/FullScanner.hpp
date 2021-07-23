//
// Created by cristobal on 7/22/21.
//

#ifndef RDFCACHEK2_FULLSCANNER_HPP
#define RDFCACHEK2_FULLSCANNER_HPP

#include "K2TreeScanner.hpp"
#include <TimeControl.hpp>
#include <k2node.h>
class FullScanner : public K2TreeScanner {
public:
  FullScanner(K2TreeMixed &k2tree, TimeControl &time_control);

  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
  void reset_scan() override;
  bool is_band() override;
  BandType get_band_type() override;
  unsigned long get_band_value() override;
  K2TreeMixed &get_tree() override;
  ~FullScanner() override;

private:
  struct k2qstate st;
  k2node_lazy_handler_naive_scan_t lazy_handler;
  K2TreeMixed &k2tree;
  TimeControl &time_control;
};

#endif // RDFCACHEK2_FULLSCANNER_HPP
