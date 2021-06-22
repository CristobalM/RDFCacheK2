#include "BandMapLazy.hpp"

BandMapLazy::BandMapLazy(const K2TreeMixed &k2tree, BType type)
    : k2tree(k2tree), type(type) {}

const std::vector<unsigned long> BandMapLazy::empty_vec =
    std::vector<unsigned long>();
const std::vector<unsigned long> &BandMapLazy::get_band(unsigned long key) {
  if (map.find(key) == map.end()) {
    if (retrieved.find(key) != retrieved.end())
      return empty_vec;

    std::vector<unsigned long> result;
    if (type == BY_COL) {
      result = k2tree.get_column(key);
    } else {
      result = k2tree.get_row(key);
    }

    if (result.size() > 0)
      map[key] = std::move(result);

    retrieved.insert(key);
  }

  if (map.find(key) == map.end())
    return empty_vec;

  return map[key];
}
