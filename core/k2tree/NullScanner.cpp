//
// Created by cristobal on 9/1/21.
//

#include <stdexcept>

#include "NullScanner.hpp"
namespace k2cache {
bool NullScanner::has_next() { return false; }
std::pair<uint64_t, uint64_t> NullScanner::next() {
  throw std::runtime_error("null scanner");
}
void NullScanner::reset_scan() { throw std::runtime_error("empty scanner"); }
uint64_t NullScanner::get_band_value() {
  throw std::runtime_error("empty scanner");
}
K2TreeMixed &NullScanner::get_tree() {
  throw std::runtime_error("empty scanner");
}
K2QStateWrapper &NullScanner::get_k2qw() {
  throw std::runtime_error("empty scanner");
}
NullScanner::~NullScanner() {}
} // namespace k2cache