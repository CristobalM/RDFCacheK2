#include "BandMap.hpp"

BandMap::BandMap(const K2TreeMixed &k2tree, BType type) {
  if (type == BY_COL) {
    k2tree.scan_points(
        [](unsigned long col, unsigned long row, void *map_ptr) {
          auto &map = *reinterpret_cast<map_t *>(map_ptr);
          map[col].push_back(row);
        },
        &map);
  } else {
    k2tree.scan_points(
        [](unsigned long col, unsigned long row, void *map_ptr) {
          auto &map = *reinterpret_cast<map_t *>(map_ptr);
          map[row].push_back(col);
        },
        &map);
  }
}

const std::vector<unsigned long> &BandMap::operator[](unsigned long key) {
  if (map.find(key) == map.end())
    return empty_vec;
  return map[key];
}

const std::vector<unsigned long> BandMap::empty_vec =
    std::vector<unsigned long>();
BandMap::BandMap() {}
