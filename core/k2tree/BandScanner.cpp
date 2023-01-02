//
// Created by cristobal on 7/22/21.
//

extern "C" {
#include "definitions.h"
#include "k2node.h"
}

#include "BandScanner.hpp"
#include "K2TreeMixed.hpp"
namespace k2cache {
bool BandScanner::has_next() {
  return static_cast<bool>(lazy_handler.has_next);
}
std::pair<uint64_t, uint64_t> BandScanner::next() {
  uint64_t result;
  k2node_report_band_next(&lazy_handler, &result);
  if (band_type == K2TreeScanner::BandType::COLUMN_BAND_TYPE) {
    return {band, result};
  }
  return {result, band};
}
BandScanner::~BandScanner() { k2node_report_band_lazy_clean(&lazy_handler); }

BandScanner::BandScanner(K2TreeMixed &k2tree, uint64_t band,
                         BandType band_type)
    : stw(k2tree.create_k2qw()), band(band), band_type(band_type),
      k2tree(k2tree) {
  if (band_type == COLUMN_BAND_TYPE) {
    k2node_report_column_lazy_init(&lazy_handler, k2tree.get_root_k2node(),
                                   stw.get_ptr(), band);
  } else {
    k2node_report_row_lazy_init(&lazy_handler, k2tree.get_root_k2node(),
                                stw.get_ptr(), band);
  }
}
void BandScanner::reset_scan() { k2node_report_band_reset(&lazy_handler); }
K2TreeMixed &BandScanner::get_tree() { return k2tree; }
uint64_t BandScanner::get_band_value() { return band; }
K2QStateWrapper &BandScanner::get_k2qw() { return stw; }
} // namespace k2cache
