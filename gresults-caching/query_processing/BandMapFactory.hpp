//
// Created by cristobal on 6/20/21.
//

#ifndef RDFCACHEK2_BANDMAPFACTORY_HPP
#define RDFCACHEK2_BANDMAPFACTORY_HPP

#include "IBandMap.hpp"
#include <K2TreeMixed.hpp>
#include <memory>
class BandMapFactory {
public:
  static std::unique_ptr<IBandMap> create_band_map(const K2TreeMixed &k2tree,
                                                   IBandMap::BType band_type);
  static std::unique_ptr<IBandMap> create_empty();
};

#endif // RDFCACHEK2_BANDMAPFACTORY_HPP
