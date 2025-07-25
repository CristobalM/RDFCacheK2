//
// Created by cristobal on 7/22/21.
//

#include "EmptyScanner.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include <stdexcept>
namespace k2cache {
bool EmptyScanner::has_next() { return false; }
std::pair<uint64_t, uint64_t> EmptyScanner::next() {
  throw std::runtime_error("empty scanner");
}
void EmptyScanner::reset_scan() {}
uint64_t EmptyScanner::get_band_value() {
  throw std::runtime_error("empty scanner");
}
K2TreeMixed &EmptyScanner::get_tree() { return k2tree; }
EmptyScanner::EmptyScanner(K2TreeMixed &k2tree)
    : stw(k2tree.create_k2qw()), k2tree(k2tree) {}
K2QStateWrapper &EmptyScanner::get_k2qw() { return stw; }
} // namespace k2cache