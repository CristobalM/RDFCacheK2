//
// Created by cristobal on 04-08-21.
//

#include <I_DataManager.hpp>
#include <caching/CacheReplacement.hpp>
#include <caching/LRUReplacementStrategy.hpp>
#include <gtest/gtest.h>

struct MockDataManager : public I_DataManager {
  std::set<unsigned long> keys;
  void remove_key(unsigned long key) override { keys.erase(key); }
  void retrieve_key(unsigned long key) override { keys.insert(key); }
};

TEST(cache_replacement_test, can_do_simple_lru_replacement_1_test) {
  MockDataManager mock_data_manager;
  std::mutex m;
  CacheReplacement<LRUReplacementStrategy> cache_replacement(
      1'000'000, &mock_data_manager, m);
  std::vector<std::pair<unsigned long, size_t>> keys_with_sizes = {
      {1, 100'000}, {2, 100'000}, {3, 100'000}, {4, 100'000}, {5, 700'000},
      {6, 700'000}, {7, 700'000}, {8, 700'000}, {9, 300'000},
  };
  int i = 0;
  std::vector<size_t> expected_sizes = {1, 2, 3, 4, 4, 1, 1, 1, 2};
  for (auto &pair : keys_with_sizes) {
    auto can_be_retrieved = cache_replacement.hit_key(pair.first, pair.second);
    ASSERT_TRUE(can_be_retrieved);
    ASSERT_EQ(mock_data_manager.keys.size(), expected_sizes[i])
        << "failed at i  = " << i;
    i++;
  }
  ASSERT_FALSE(cache_replacement.hit_key(10, 1000001));
}
