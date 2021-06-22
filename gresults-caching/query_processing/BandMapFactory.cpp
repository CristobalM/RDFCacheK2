//
// Created by cristobal on 6/20/21.
//

#include "BandMapFactory.hpp"
#include "BandMap.hpp"
#include "BandMapLazy.hpp"
std::unique_ptr<IBandMap>
BandMapFactory::create_band_map(const K2TreeMixed &k2tree,
                                IBandMap::BType band_type) {
  if (k2tree.size() > 10'000'000) {
    return std::make_unique<BandMapLazy>(k2tree, band_type);
  }
  return std::make_unique<BandMap>(k2tree, band_type);
}
std::unique_ptr<IBandMap> BandMapFactory::create_empty() {
  return std::unique_ptr<BandMap>();
}
