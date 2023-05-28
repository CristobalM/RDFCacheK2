//
// Created by cristobal on 26-06-22.
//

#include "CacheContainerFactory.hpp"
#include "CacheContainerImpl.hpp"
#include "manager/PCMFactory.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "replacement/CacheReplacementFactory.hpp"
namespace k2cache {
std::unique_ptr<CacheContainer>
CacheContainerFactory::create(const CacheArgs &args) {
  auto pcm = PCMFactory::create(args);
  auto *pcm_ptr = pcm.get();
  auto container = std::make_unique<CacheContainerImpl>(
      std::move(pcm), NodeIdsManagerFactory::create(args),
      CacheReplacementFactory::create_cache_replacement(
          args.memory_budget_bytes, pcm_ptr, args.replacement_strategy),
      args.replacement_strategy, args.has_sort_results, args.timeout_ms);
  if (args.replacement_strategy ==
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING) {
    std::cout << "Loading all predicates..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    pcm_ptr->load_all_predicates();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Done loading all predicates, took " << duration.count()
              << " seconds" << std::endl;
  }

  return container;
}

} // namespace k2cache
