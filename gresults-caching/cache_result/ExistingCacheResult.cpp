//
// Created by Cristobal Miranda, 2020
//

#include "ExistingCacheResult.hpp"

ExistingCacheResult::ExistingCacheResult(
    std::unique_ptr<GraphResult> &&graph_result) {
  this->graph_result = std::move(graph_result);
}

bool ExistingCacheResult::exists() { return true; }

std::string ExistingCacheResult::as_string() { return std::string(); }
