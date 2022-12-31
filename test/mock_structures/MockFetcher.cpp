//
// Created by cristobal on 03-10-22.
//

#include "MockFetcher.hpp"
namespace k2cache{

PredicateFetchResult MockFetcher::fetch_k2tree(uint64_t predicate_index) {
  auto it = data.find(predicate_index);
  if (it == data.end()){
    return {false, nullptr};
  }
  return {true, it->second.get()};
}

PredicateFetchResult
MockFetcher::fetch_k2tree_if_loaded(uint64_t predicate_index) {
  return fetch_k2tree(predicate_index);
}
MockFetcher::MockFetcher() {
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  for(uint64_t i = 1; i <= 5; i++){
    data[i] = std::make_unique<K2TreeMixed>(config);
    auto st =data[i]->create_k2qw();
    for(uint64_t j = 1; j <= 100; j++){
      data[i]->insert(j, j, st);
    }
  }
}
}