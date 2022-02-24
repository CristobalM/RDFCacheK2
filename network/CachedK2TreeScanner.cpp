//
// Created by cristobal on 2/21/22.
//

#include <stdexcept>

#include "CachedK2TreeScanner.hpp"
void CachedK2TreeScanner::Builder::add_point(unsigned long col,
                                             unsigned long row) {
  points_plain.push_back(col);
  points_plain.push_back(row);
}
std::unique_ptr<K2TreeScanner> CachedK2TreeScanner::Builder::get_scanner() {
  auto out =
      std::make_unique<CachedK2TreeScanner>( std::move(points_plain));
  return out;
}
CachedK2TreeScanner::Builder::Builder(
                                      unsigned long expected_points)
    {
  points_plain.reserve(expected_points);
}
CachedK2TreeScanner::Builder::Builder()
    : Builder(1000000) {}

bool CachedK2TreeScanner::has_next() {
  return 2 * current_i < points_plain.size();
}

std::pair<unsigned long, unsigned long> CachedK2TreeScanner::next() {
  unsigned long col = points_plain[2*current_i];
  unsigned long row = points_plain[2*current_i + 1];
  ++current_i;
  return {col, row};
}
void CachedK2TreeScanner::reset_scan() {
  current_i = 0;
}
unsigned long CachedK2TreeScanner::get_band_value() {
  throw std::runtime_error("get_band_value not available for cached scanner");
}
K2TreeMixed &CachedK2TreeScanner::get_tree() {
    throw std::runtime_error("get_tree not available for cached scanner");
}
K2QStateWrapper &CachedK2TreeScanner::get_k2qw() {
  throw std::runtime_error("get_k2qw not available for cached scanner");
}
unsigned long CachedK2TreeScanner::get_size() {
  return points_plain.size() / 2;
}
CachedK2TreeScanner::CachedK2TreeScanner(std::vector<unsigned long> &&points_plain)
    : points_plain(std::move(points_plain)), current_i(0) {}
bool CachedK2TreeScanner::has_point(unsigned long, unsigned long) {
  throw std::runtime_error("has_point not available for cached scanner");
}
bool CachedK2TreeScanner::is_cached() { return true; }
