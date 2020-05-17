//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXISTINGCACHERESULT_HPP
#define RDFCACHEK2_EXISTINGCACHERESULT_HPP

#include <chrono>
#include <memory>

#include "CacheResult.hpp"

#include "GraphResult.hpp"

using timestamp_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

class ExistingCacheResult : CacheResult {
  timestamp_t timestamp;
  std::unique_ptr<GraphResult> graph_result;

public:
  explicit ExistingCacheResult(std::unique_ptr<GraphResult> &&graph_result);

  bool exists() override;
  std::string as_string() override;
};

#endif // RDFCACHEK2_EXISTINGCACHERESULT_HPP
