//
// Created by cristobal on 12-10-22.
//
#include "cache_test_util.hpp"
#include "util_algorithms/fisher_yates.hpp"
#include "nodeids/NodeIdsManagerImpl.hpp"
#include "nodeids/NodesSequence.hpp"
#include <gtest/gtest.h>
#include <serialization_util.hpp>

using namespace k2cache;

TEST(nodeids_manager_impl_test, can_get_id_simple_input) {
  auto nis_vec =
      std::vector<unsigned long>{9, 323, 1322, 9433, 54345, 9595955, 34343333};
  auto tdnis = boilerplate_nis_from_vec(nis_vec);
  auto &nis = tdnis.nim;
  for (auto i = 0UL; i < nis_vec.size(); i++) {
    ASSERT_EQ(i, nis->get_id(nis_vec[i]));
  }
}

TEST(nodeids_manager_impl_test, can_get_real_id_simple_input) {
  auto nis_vec =
      std::vector<unsigned long>{9, 323, 1322, 9433, 54345, 9595955, 34343333};
  auto tdnis = boilerplate_nis_from_vec(nis_vec);
  auto &nis = tdnis.nim;
  for (auto i = 0UL; i < nis_vec.size(); i++) {
    ASSERT_EQ(nis_vec[i], nis->get_real_id(i, nullptr));
  }
}

TEST(nodeids_manager_impl_test, can_get_id_random_fys_input_10) {
  for (auto t = 0; t < 10; t++) {
    auto nis_vec = fisher_yates(10000, 1UL << 32);
    std::sort(nis_vec.begin(), nis_vec.end());
    auto tdnis = boilerplate_nis_from_vec(nis_vec);
    auto &nis = tdnis.nim;
    for (auto i = 0UL; i < nis_vec.size(); i++) {
      ASSERT_EQ(i, nis->get_id(nis_vec[i]));
    }
  }
}

TEST(nodeids_manager_impl_test, can_get_real_id_random_fys_input_10) {
  for (auto t = 0; t < 10; t++) {
    auto nis_vec = fisher_yates(100000, 1UL << 32);
    std::sort(nis_vec.begin(), nis_vec.end());
    auto tdnis = boilerplate_nis_from_vec(nis_vec);
    auto &nis = tdnis.nim;
    for (auto i = 0UL; i < nis_vec.size(); i++) {
      ASSERT_EQ(nis_vec[i], nis->get_real_id(i, nullptr));
    }
  }
}
