//
// Created by cristobal on 9/1/21.
//

#include <stdexcept>

#include "NullScanner.hpp"
bool NullScanner::has_next() { return false; }
std::pair<unsigned long, unsigned long> NullScanner::next() {
  throw std::runtime_error("null scanner");
}
void NullScanner::reset_scan() { throw std::runtime_error("null scanner"); }
unsigned long NullScanner::get_band_value() {
  throw std::runtime_error("null scanner");
}
K2TreeMixed &NullScanner::get_tree() {
  throw std::runtime_error("null scanner");
}
K2QStateWrapper &NullScanner::get_k2qw() {
  throw std::runtime_error("null scanner");
}
NullScanner::~NullScanner() {}
unsigned long NullScanner::get_size() {
  throw std::runtime_error("null scanner");
}
bool NullScanner::has_point(unsigned long, unsigned long) {
  throw std::runtime_error("null scanner");
}
