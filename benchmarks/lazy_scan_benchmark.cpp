//
// Created by cristobal on 02-12-21.
//

#include "manager/PCMFactory.hpp"
#include "manager/PredicatesCacheManager.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>

using namespace k2cache;
namespace fs = std::filesystem;
void run_benchmarks(PredicatesCacheManager &pcm);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Expected exactly one argument: k2tree serialized file"
              << std::endl;
    exit(1);
  }

  fs::path k2tree_path(argv[1]);

  if (!fs::exists(k2tree_path)) {
    std::cerr << "Given filename '" << k2tree_path << "' doesn't exist"
              << std::endl;
    exit(1);
  }

  auto pcm = PCMFactory::create(k2tree_path);

  run_benchmarks(*pcm);
}
void run_benchmarks(PredicatesCacheManager &pcm) {
  pcm.load_all_predicates();

  const auto &predicates_ids =
      pcm.get_predicates_index_cache().get_predicates_ids();
  auto start = std::chrono::high_resolution_clock::now();
  for (auto predicate_id : predicates_ids) {
    auto fetch_result =
        pcm.get_predicates_index_cache().fetch_k2tree(predicate_id);
    auto &k2tree = fetch_result.get_mutable();
    auto scanner = k2tree.create_full_scanner();
    while (scanner->has_next()) {
      scanner->next();
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "Full scan took: " << duration.count() << " microseconds"
            << std::endl;
}
