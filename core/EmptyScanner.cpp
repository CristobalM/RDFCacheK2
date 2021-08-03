//
// Created by cristobal on 7/22/21.
//

#include "EmptyScanner.hpp"
#include "K2TreeMixed.hpp"
#include <stdexcept>
bool EmptyScanner::has_next() { return false; }
std::pair<unsigned long, unsigned long> EmptyScanner::next() {
  throw std::runtime_error("empty scanner");
}
void EmptyScanner::reset_scan() {}
unsigned long EmptyScanner::get_band_value() {
  throw std::runtime_error("empty scanner");
}
K2TreeMixed &EmptyScanner::get_tree() { return k2tree; }
EmptyScanner::EmptyScanner(K2TreeMixed &k2tree)
    : stw(k2tree.create_k2qw()), k2tree(k2tree) {}
K2QStateWrapper &EmptyScanner::get_k2qw() { return stw; }
