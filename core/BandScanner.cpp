//
// Created by cristobal on 7/22/21.
//

extern "C" {
#include <definitions.h>
#include <k2node.h>
}

#include "BandScanner.hpp"
#include <K2TreeMixed.hpp>

bool BandScanner::has_next() { return lazy_handler.has_next; }
std::pair<unsigned long, unsigned long> BandScanner::next() {
  uint64_t result;
  k2node_report_band_next(&lazy_handler, &result);
  if (band_type == K2TreeScanner::BandType::COLUMN_BAND_TYPE) {
    return {band, result};
  }
  time_control.tick_only_count();
  return {result, band};
}
BandScanner::~BandScanner() {
  k2node_report_band_lazy_clean(&lazy_handler);
  clean_k2qstate(&st);
}

BandScanner::BandScanner(K2TreeMixed &k2tree, unsigned long band,
                         BandType band_type, TimeControl &time_control)
    : band(band), band_type(band_type), k2tree(k2tree),
      time_control(time_control) {
  auto *base_st = k2tree.get_k2qstate();
  init_k2qstate(&st, base_st->k2tree_depth, base_st->qs.max_nodes_count,
                base_st->cut_depth);

  if (band_type == COLUMN_BAND_TYPE) {
    k2node_report_column_lazy_init(&lazy_handler, k2tree.get_root_k2node(), &st,
                                   band);
  } else {
    k2node_report_row_lazy_init(&lazy_handler, k2tree.get_root_k2node(), &st,
                                band);
  }
}
void BandScanner::reset_scan() { k2node_report_band_reset(&lazy_handler); }
K2TreeMixed &BandScanner::get_tree() { return k2tree; }
unsigned long BandScanner::get_band_value() { return band; }
