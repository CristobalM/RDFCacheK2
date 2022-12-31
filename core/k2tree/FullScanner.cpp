//
// Created by cristobal on 7/22/21.
//

extern "C" {
#include "definitions.h"
#include "k2node.h"
}

#include "FullScanner.hpp"
#include "K2TreeMixed.hpp"
namespace k2cache {
bool FullScanner::has_next() {
  return static_cast<bool>(lazy_handler.has_next);
}
std::pair<uint64_t, uint64_t> FullScanner::next() {
  pair2dl_t result;
  k2node_naive_scan_points_lazy_next(&lazy_handler, &result);
  return {result.col, result.row};
}

FullScanner::FullScanner(K2TreeMixed &k2tree)
    : stw(k2tree.create_k2qw()), k2tree(k2tree) {
  k2node_naive_scan_points_lazy_init(k2tree.get_root_k2node(), stw.get_ptr(),
                                     &lazy_handler);
}
FullScanner::~FullScanner() {
  k2node_naive_scan_points_lazy_clean(&lazy_handler);
}
void FullScanner::reset_scan() {
  k2node_naive_scan_points_lazy_reset(&lazy_handler);
}
K2TreeMixed &FullScanner::get_tree() { return k2tree; }
uint64_t FullScanner::get_band_value() {
  throw std::runtime_error("Not band");
}
K2QStateWrapper &FullScanner::get_k2qw() { return stw; }
} // namespace k2cache
