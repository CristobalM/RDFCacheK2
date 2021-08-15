//
// Created by cristobal on 04-08-21.
//

#include <I_DataManager.hpp>
#include <caching/CacheReplacement.hpp>
#include <caching/FrequencyReplacementStrategy.hpp>
#include <caching/LRUReplacementStrategy.hpp>
#include <gtest/gtest.h>

struct MockDataManager : public I_DataManager {
  std::set<unsigned long> keys;
  void remove_key(unsigned long key) override { keys.erase(key); }
  void retrieve_key(unsigned long key) override { keys.insert(key); }
};

TEST(cache_replacement_test, can_do_simple_lru_replacement_1_test) {
  MockDataManager mock_data_manager;
  CacheReplacement<LRUReplacementStrategy> cache_replacement(
      1'000'000, &mock_data_manager);
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

TEST(cache_replacement_test, can_do_simple_frequency_replacement_1_test) {
  MockDataManager mock_data_manager;
  CacheReplacement<FrequencyReplacementStrategy> cache_replacement(
      1'000'000, &mock_data_manager);
  std::vector<std::pair<unsigned long, size_t>> keys_with_sizes = {
      {1, 100'000},  {1, 100'000},  {1, 100'000},  {1, 100'000},

      {2, 100'000},  {2, 100'000},  {2, 100'000},

      {3, 100'000},  {3, 100'000},

      {4, 100'000},

      {5, 700'000},  {6, 700'000},  {7, 700'000},  {8, 700'000},
      {9, 300'000},  {10, 700'000}, {10, 700'000}, {10, 700'000},
      {10, 700'000}, {10, 700'000}, {11, 100'000}, {12, 900'000},

  };
  int i = 0;
  std::vector<size_t> expected_sizes = {1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4,
                                        4, 4, 4, 5, 5, 4, 4, 4, 4, 4, 4};

  std::vector<bool> can_be_retrieved_values = {
      true,  true,  true,  true, true,  true, true, true, true, true,  false,
      false, false, false, true, false, true, true, true, true, false, false};
  std::cout << "expected sizes length: " << expected_sizes.size() << std::endl;
  for (auto &pair : keys_with_sizes) {
    std::cout << "hitting key " << pair.first << " with size " << pair.second
              << std::endl;

    auto can_be_retrieved = cache_replacement.hit_key(pair.first, pair.second);

    ASSERT_EQ(can_be_retrieved, can_be_retrieved_values[i])
        << "failed at i = " << i;
    ASSERT_EQ(mock_data_manager.keys.size(), expected_sizes[i])
        << "failed at i  = " << i;

    // ensure that keys 1 and 2 aren't dropped
    if (i == 10) {
      ASSERT_NE(mock_data_manager.keys.find(1), mock_data_manager.keys.end());
      ASSERT_NE(mock_data_manager.keys.find(2), mock_data_manager.keys.end());
      ASSERT_NE(mock_data_manager.keys.find(3), mock_data_manager.keys.end());
    }

    i++;
  }
  ASSERT_FALSE(cache_replacement.hit_key(10, 1000001));
}

TEST(cache_replacement_test, map_mutable_1) {
  std::map<unsigned long, long> in_use;

  auto mark_using = [&](unsigned long key) {
    auto it = in_use.find(key);
    if (it == in_use.end()) {
      in_use[key] = 1;
      return;
    }
    it->second++;
  };
  auto mark_ready = [&](unsigned long key) {
    auto it = in_use.find(key);
    if (it == in_use.end()) {
      return;
    }
    it->second--;
    if (it->second <= 0) {
      in_use.erase(it);
    }
  };

  for (unsigned long i = 0; i < 100; i++) {
    mark_using(i);
    mark_using(i);
  }
  for (unsigned long i = 0; i < 100; i++) {
    ASSERT_EQ(in_use[i], 2);
  }

  for (unsigned long i = 0; i < 100; i++) {
    mark_ready(i);
  }
  for (unsigned long i = 0; i < 100; i++) {
    ASSERT_EQ(in_use[i], 1);
  }

  for (unsigned long i = 0; i < 100; i++) {
    mark_ready(i);
  }

  for (unsigned long i = 0; i < 100; i++) {
    ASSERT_EQ(in_use.find(i), in_use.end());
  }
  ASSERT_EQ(in_use.size(), 0);
}