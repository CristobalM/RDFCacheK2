//
// Created by cristobal on 11-12-22.
//

#ifndef RDFCACHEK2_SEARCH_PATHS_HPP
#define RDFCACHEK2_SEARCH_PATHS_HPP

#include "DirectedPath.hpp"
#include "manager/PredicatesCacheManager.hpp"
namespace k2cache {

/**
 * Search for at most maxNumber n-DirectedPaths,
 * the search will stop earlier if there are less than maxNumber
 * paths available
 * @param pcm
 * @param n
 * @param maxNumber
 * @return n DirectedPaths
 */
std::vector<DirectedPath> find_n_paths(
    PredicatesCacheManager &pcm,
    int n,
    int maxNumber
    );

}

#endif // RDFCACHEK2_SEARCH_PATHS_HPP
