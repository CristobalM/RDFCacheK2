//
// Created by cristobal on 7/22/21.
//

extern "C" {
#include <definitions.h>
#include <k2node.h>
}

#include "FullScanner.hpp"
#include "K2TreeMixed.hpp"

bool FullScanner::has_next() {
  return static_cast<bool>(lazy_handler.has_next);
}
std::pair<unsigned long, unsigned long> FullScanner::next() {
  pair2dl_t result;
  k2node_naive_scan_points_lazy_next(&lazy_handler, &result);
  time_control.tick_only_count();
  return {result.col, result.row};
}

FullScanner::FullScanner(K2TreeMixed &k2tree, TimeControl &time_control)
    : k2tree(k2tree), time_control(time_control) {
  auto *base_st = k2tree.get_k2qstate();
  init_k2qstate(&st, base_st->k2tree_depth, base_st->qs.max_nodes_count,
                base_st->cut_depth);
  k2node_naive_scan_points_lazy_init(k2tree.get_root_k2node(), &st,
                                     &lazy_handler);
}
FullScanner::~FullScanner() {
  k2node_naive_scan_points_lazy_clean(&lazy_handler);
  clean_k2qstate(&st);
}
void FullScanner::reset_scan() {
  k2node_naive_scan_points_lazy_reset(&lazy_handler);
}
K2TreeMixed &FullScanner::get_tree() { return k2tree; }
unsigned long FullScanner::get_band_value() {
  throw std::runtime_error("Not band");
}
