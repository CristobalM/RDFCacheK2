//
// Created by cristobal on 12-10-22.
//
#include "cache_test_util.hpp"
#include "fisher_yates.hpp"
#include "nodeids/NodeIdsManagerImpl.hpp"
#include "nodeids/NodesSequence.hpp"
#include <gtest/gtest.h>
#include <serialization_util.hpp>

using namespace k2cache;

static std::string zero_data_str_content() {
  std::stringstream ss;
  write_u64(ss, 0);
  return ss.str();
}

static std::unique_ptr<NIMDataHolders>
no_logs_static_ni_dh(std::string plain_ni) {
  auto dh = std::make_unique<NIMDataHolders>();
  dh->plain = std::move(plain_ni);
  dh->logs = zero_data_str_content();
  dh->logs_counter = zero_data_str_content();
  dh->mapped = zero_data_str_content();
  return dh;
}

struct TD_Nis {
  std::unique_ptr<NIMDataHolders> nim_dh;
  std::unique_ptr<NodeIdsManager> nim;
};

static TD_Nis
boilerplate_nis_from_vec(const std::vector<unsigned long> &data_vec) {
  auto ss = build_node_ids_seq_mem(data_vec);
  auto dh = no_logs_static_ni_dh(ss.str());
  auto nis = mock_nis(*dh);
  TD_Nis out;
  out.nim_dh = std::move(dh);
  out.nim = std::move(nis);
  return out;
}

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
    ASSERT_EQ(nis_vec[i], nis->get_real_id(i));
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
      ASSERT_EQ(nis_vec[i], nis->get_real_id(i));
    }
  }
}
