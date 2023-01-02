//
// Created by cristobal on 11-12-22.
//

#ifndef RDFCACHEK2_SEARCH_PATHS_HPP
#define RDFCACHEK2_SEARCH_PATHS_HPP

#include "DirectedPath.hpp"
#include "manager/PredicatesCacheManager.hpp"
namespace k2cache {

/**
 * Search for at most max_number n-DirectedPaths,
 * the search will stop earlier if there are less than max_number
 * paths available
 * @param pcm
 * @param n
 * @param max_number
 * @return n DirectedPaths
 */
std::vector<std::pair<uint64_t, uint64_t>> find_n_paths(
    PredicatesCacheManager &pcm,
    int n,
    int max_number
    );
}

#endif // RDFCACHEK2_SEARCH_PATHS_HPP
