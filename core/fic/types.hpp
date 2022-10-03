//
// Created by cristobal on 03-10-22.
//

#ifndef RDFCACHEK2_TYPES_HPP
#define RDFCACHEK2_TYPES_HPP
#include "I_CachedPredicateSource.hpp"
#include <memory>
#include <unordered_map>
namespace k2cache::fic::types{
using cache_map_t =
    std::unordered_map<unsigned long,
                       std::unique_ptr<I_CachedPredicateSource>>;
}
#endif // RDFCACHEK2_TYPES_HPP
