//
// Created by cristobal on 19-06-22.
//

#include "I_FileRWHandler.hpp"
#include "cache_test_util.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "server/session/UpdaterSession.hpp"
#include <gtest/gtest.h>
#include <vector>
using namespace k2cache;
TEST(node_ids_dyn_mapper_test, can_map_big_ids_to_smaller) {
  auto nis = mock_nis();
  auto pcm = basic_pcm(std::move(nis));
  static constexpr auto total_points = 10000UL;

  auto update_config = UpdaterSession::get_initial_update_k2tree_config();
  auto predicate_id = 343UL;
  K2TreeMixed k2tree(update_config);
  K2TreeBulkOp op(k2tree);

  for (auto i = 0UL; i < total_points; i++) {
    auto j =  i + 1;
    op.insert(j, j);
  }
  auto updates = std::vector<K2TreeUpdates>{
      {predicate_id, &k2tree, nullptr},
  };
  pcm->get_updates_logger().log(updates);
  ASSERT_TRUE(pcm->get_updates_logger().has_predicate_stored(predicate_id));

//  ASSERT_TRUE(
//      pcm->get_predicates_index_cache().has_predicate_active(predicate_id));
  auto f = pcm->get_predicates_index_cache().fetch_k2tree(predicate_id);
  ASSERT_TRUE(f.exists());
  auto &tree = f.get_mutable();
  ASSERT_EQ(tree.size(), total_points);
  for (auto i = 0UL; i < total_points; i++) {
    auto j =   i + 1;
    ASSERT_TRUE(tree.has(j, j));
  }
}
